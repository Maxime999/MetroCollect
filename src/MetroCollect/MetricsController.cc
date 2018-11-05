//
// MetricsController.cc
//
// Created on July 24th 2018
//
// Copyright 2018 CFM (www.cfm.fr)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <algorithm>
#include <cmath>
#include <thread>

#if GPERFTOOLS_CPU_PROFILE
#include <gperftools/profiler.h>
#endif

#include "MetricsController.h"


namespace MetroCollect {
	MetricsController::MetricsController(MetricsControllerDelegate& delegate) noexcept :
		delegate_(delegate),
		sourceInterests_(MetricsSource::makeSourceInterests(false)),
		sendValues_(MetricsController::defaultSendValues),
		sendStats_(MetricsController::defaultSendStats),
		isCollecting_(false),
		convertToUnitsPerSecond_(MetricsController::defaultConvertToUnitsPerSecond),
		processingWindowLength_(MetricsController::defaultProcessingWindowLength),
		processingWindowOverlap_(MetricsController::defaultProcessingWindowOverlap),
		currentMetrics_(std::make_unique<MetricsDataArray>(this->sourceInterests_)),
		previousMetrics_(std::make_unique<MetricsDataArray>(this->sourceInterests_)),
		metricsValues_(MetricsController::defaultProcessingWindowLength, MetricsDiffArray(this->sourceInterests_))
	{
		this->setSamplingInterval(MetricsController::defaultSamplingInterval);
	}


	MetricsControllerDelegate& MetricsController::delegate() const noexcept {
		return this->delegate_;
	}


	bool MetricsController::isCollecting() const noexcept {
		return this->isCollecting_;
	}


	bool MetricsController::sendValues() const noexcept {
		return this->sendValues_;
	}

	bool MetricsController::sendStats() const noexcept {
		return this->sendStats_;
	}

	const std::vector<size_t>& MetricsController::requestedMetrics() const noexcept {
		return this->requestedMetrics_;
	}

	std::chrono::milliseconds MetricsController::samplingInterval() const noexcept {
		return this->samplingInterval_;
	}

	size_t MetricsController::processingWindowLength() const noexcept {
		return this->processingWindowLength_;
	}

	size_t MetricsController::processingWindowOverlap() const noexcept {
		return this->processingWindowOverlap_;
	}

	bool MetricsController::convertToUnitsPerSeconds() const noexcept {
		return this->convertToUnitsPerSecond_;
	}


	void MetricsController::setSendValues(bool sendValues) noexcept {
		this->sendValues_ = sendValues;
	}

	void MetricsController::setSendStats(bool sendStats) noexcept {
		this->sendStats_ = sendStats;
	}

	void MetricsController::setRequestedMetrics(const MetricsArray<Statistics::Stats>& requestedMetrics) noexcept {
		if (this->isCollecting_)
			return;

		std::set<size_t> indexes;
		std::set<size_t> allIndexes;
		*this->sourceInterests_ = *requestedMetrics.sourceInterests();
		for (size_t i = 0; i < Statistics::extraCount; i++) {
			indexes.clear();
			for (size_t j = 0; j < requestedMetrics.size(); j++) {
				if (requestedMetrics[j] & Statistics::StatsDependencies[i]) {
					indexes.insert(j);
					allIndexes.insert(j);
				}
			}

			switch (Statistics::StatsIndex(i)) {
				case Statistics::StatsIndexMin:
					this->metricsStatsIntermediate_.minIndexes.setIndexes(indexes);
					this->metricsStats_.min.setIndexes(indexes);
					break;
				case Statistics::StatsIndexMax:
					this->metricsStatsIntermediate_.maxIndexes.setIndexes(indexes);
					this->metricsStats_.max.setIndexes(indexes);
					break;
				case Statistics::StatsIndexAverage:
					this->metricsStats_.average.setIndexes(indexes);
					break;
				case Statistics::StatsIndexStdDev:
					this->metricsStats_.stdDev.setIndexes(indexes);
					break;
				case Statistics::StatsIndexIntermediateSum:
					this->metricsStatsIntermediate_.sum.setIndexes(indexes);
					break;
				case Statistics::StatsIndexIntermediateSumSquared:
					this->metricsStatsIntermediate_.sumSquared.setIndexes(indexes);
					break;
			}
		}

		this->requestedMetrics_.clear();
		for (size_t i : allIndexes)
			this->requestedMetrics_.push_back(i);
	}

	void MetricsController::setSamplingInterval(std::chrono::milliseconds interval) noexcept {
		if (this->isCollecting_)
			return;

		this->samplingInterval_ = interval;
		if (interval != 0ms)
			this->unitsPerSecondFactor_ = 1000.0 / interval.count();
		else
			this->unitsPerSecondFactor_ = 1.0;
	}

	void MetricsController::setProcessingWindow(size_t length, size_t overlap) noexcept {
		if (isCollecting_)
			return;

		if (length != this->processingWindowLength_) {
			this->processingWindowLength_ = length;
			this->metricsValues_.reset(length, MetricsDiffArray(this->sourceInterests_));
		}

		if (overlap >= length)
			overlap = length - 1;
		if (overlap != this->processingWindowOverlap_)
			this->processingWindowOverlap_ = overlap;
	}

	void MetricsController::setConvertToUnitsPerSeconds(bool convertToUnitsPerSecond) noexcept {
		if (isCollecting_)
			return;

		this->convertToUnitsPerSecond_ = convertToUnitsPerSecond;
	}


	void MetricsController::collectMetrics() {
		if (this->isCollecting_)
			return;
#if GPERFTOOLS_CPU_PROFILE
		ProfilerStart("/tmp/aa.prof");
#endif
		this->isCollecting_ = true;

		bool cachedSendStats = this->sendStats_;
		size_t processingWindowIndex = 0;
		auto startTime = std::chrono::steady_clock::now();

		this->currentMetrics_->updateData();
		while (true) {
			std::this_thread::sleep_for(this->samplingInterval_ - (std::chrono::steady_clock::now() - startTime));
			startTime = std::chrono::steady_clock::now();

			this->updateMetrics();
			if (cachedSendStats)
				this->updateIterativeStats();

			if (this->sendValues_)
				this->delegate_.metricsContollerCollectedMetricsValues(*this, this->metricsValues_.back(), *this->previousMetrics_, *this->currentMetrics_);

			processingWindowIndex++;
			if (processingWindowIndex >= this->processingWindowLength_) {
				if (cachedSendStats) {
					this->computeFinalStats();
					this->delegate_.metricsContollerCollectedMetricsStats(*this, this->metricsStats_);
				}
				if (this->delegate_.metricsContollerShouldStopCollectingMetrics(*this)) {
					this->metricsValues_.reset();
					if (cachedSendStats)
						this->resetIterativeStats();
					this->isCollecting_ = false;
#if GPERFTOOLS_CPU_PROFILE
					ProfilerStop();
#endif
					return;
				}
				this->metricsValues_.moveBegin(this->processingWindowOverlap_);
				processingWindowIndex = this->processingWindowOverlap_;

				if (cachedSendStats)
					this->resetIterativeStats();
				cachedSendStats = this->sendStats_;
#if GPERFTOOLS_CPU_PROFILE
				ProfilerFlush();
#endif
			}
		}
	}

	bool MetricsController::isMetricNull(size_t index) const {
		return ((*this->currentMetrics_)[index] == 0 && (*this->previousMetrics_)[index] == 0);
	}


	void MetricsController::updateMetrics() {
		std::swap(this->currentMetrics_, this->previousMetrics_);
		this->currentMetrics_->updateData();
		this->metricsValues_.moveEnd(1);
		this->metricsValues_.back().computeDiff(*this->currentMetrics_, *this->previousMetrics_, this->convertToUnitsPerSecond_ ? this->unitsPerSecondFactor_ : 1.0);
	}

	void MetricsController::updateIterativeStats() {
		auto& newPoint = this->metricsValues_.back();
		auto newPointAbsoluteIndex = this->metricsValues_.absoluteIndex(this->metricsValues_.size() - 1);

		for (auto& val : this->metricsStatsIntermediate_.minIndexes.indexedValues()) {
			if (newPoint[val.index] < this->metricsValues_.atAbsoluteIndex(val.value)[val.index])
				val.value = newPointAbsoluteIndex;
		}

		for (auto& val : this->metricsStatsIntermediate_.maxIndexes.indexedValues()) {
			if (newPoint[val.index] > this->metricsValues_.atAbsoluteIndex(val.value)[val.index])
				val.value = newPointAbsoluteIndex;
		}

		for (auto& val : this->metricsStatsIntermediate_.sum.indexedValues()) {
			val.value += newPoint[val.index];
		}

		for (auto& val : this->metricsStatsIntermediate_.sumSquared.indexedValues()) {
			val.value += newPoint[val.index] * newPoint[val.index];
		}
	}

	void MetricsController::computeFinalStats() {
		this->metricsStats_.forEach([&](auto& array, Statistics::Stats ) {
			array.setStartTime(this->metricsValues_.front().timestamp());
			array.setEndTime(this->metricsValues_.back().timestamp());
		});

		for (const auto& val : this->metricsStatsIntermediate_.minIndexes.indexedValues())
			this->metricsStats_.min[val.index] = this->metricsValues_.atAbsoluteIndex(val.value)[val.index];

		for (const auto& val : this->metricsStatsIntermediate_.maxIndexes.indexedValues())
			this->metricsStats_.max[val.index] = this->metricsValues_.atAbsoluteIndex(val.value)[val.index];

		for (const auto& val : this->metricsStatsIntermediate_.sum.indexedValues())
			this->metricsStats_.average[val.index] = val.value / static_cast<double>(this->processingWindowLength_);

		for (const auto& val : this->metricsStats_.average.indexedValues()) {
			double variance = this->metricsStatsIntermediate_.sumSquared[val.index] / static_cast<double>(this->processingWindowLength_) - val.value * val.value;
			if (variance < 1e-4)
				this->metricsStats_.stdDev[val.index] = 0;
			else
				this->metricsStats_.stdDev[val.index] = sqrt(variance);
		}
	}

	void MetricsController::resetIterativeStats() {
		auto firstIndex = this->metricsValues_.absoluteIndex(0);
		if (this->metricsValues_.size() == 0) {
			for (auto& val : this->metricsStatsIntermediate_.minIndexes.indexedValues())
				val.value = firstIndex;
			for (auto& val : this->metricsStatsIntermediate_.maxIndexes.indexedValues())
				val.value = firstIndex;
			for (auto& val : this->metricsStatsIntermediate_.sum.indexedValues())
				val.value = 0;
			for (auto& val : this->metricsStatsIntermediate_.sumSquared.indexedValues())
				val.value = 0;
		}
		else {
			for (auto& val : this->metricsStatsIntermediate_.minIndexes.indexedValues()) {
				if (!this->metricsValues_.absoluteIndexIsInBounds(val.value)) {
					val.value = firstIndex;
					for (size_t i = 0; i < this->metricsValues_.size(); i++) {
						if (this->metricsValues_[i][val.index] < this->metricsValues_.atAbsoluteIndex(val.value)[val.index])
							val.value = this->metricsValues_.absoluteIndex(i);
					}
				}
			}

			for (auto& val : this->metricsStatsIntermediate_.maxIndexes.indexedValues()) {
				if (!this->metricsValues_.absoluteIndexIsInBounds(val.value)) {
					val.value = firstIndex;
					for (size_t i = 0; i < this->metricsValues_.size(); i++) {
						if (this->metricsValues_[i][val.index] > this->metricsValues_.atAbsoluteIndex(val.value)[val.index])
							val.value = this->metricsValues_.absoluteIndex(i);
					}
				}
			}

			if (this->processingWindowOverlap_ * 2 <= this->processingWindowLength_) {
				for (auto& val : this->metricsStatsIntermediate_.sum.indexedValues()) {
					val.value = 0;
					for (size_t i = 0; i < this->metricsValues_.size(); i++)
						val.value += this->metricsValues_[i][val.index];
				}

				for (auto& val : this->metricsStatsIntermediate_.sumSquared.indexedValues()) {
					val.value = 0;
					for (size_t i = 0; i < this->metricsValues_.size(); i++)
						val.value += this->metricsValues_[i][val.index] * this->metricsValues_[i][val.index];
				}
			}
			else {
				for (auto& val : this->metricsStatsIntermediate_.sum.indexedValues()) {
					for (long i = -1; i >= static_cast<long>(this->processingWindowOverlap_ - this->processingWindowLength_); i--)
						val.value -= this->metricsValues_[i][val.index];
				}

				for (auto& val : this->metricsStatsIntermediate_.sumSquared.indexedValues()) {
					for (long i = -1; i >= static_cast<long>(this->processingWindowOverlap_ - this->processingWindowLength_); i--)
						val.value -= this->metricsValues_[i][val.index] * this->metricsValues_[i][val.index];
				}
			}
		}
	}


	Statistics::Stats Statistics::statsFromName(const std::string_view& statName) {
		if (statName == Statistics::nameStatsAll)
			return StatsAll;
		auto itr = std::find(Statistics::names.begin(), Statistics::names.end(), statName);
		size_t index = std::distance(Statistics::names.begin(), itr);
		if (index < Statistics::names.size())
			return allStats[index];
		else
			return StatsNone;
	}


	Statistics::Stats Statistics::statsRequiredForStat(Stats stat) {
		for (size_t i = 0; i < count; i++) {
			if (stat & StatsDependencies[i])
				stat |= allStats[i];
		}
		return stat;
	}
}
