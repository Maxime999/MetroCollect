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

#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include <CircularArray/CircularArray.h>

#include "MetricsDataArray.h"
#include "MetricsDiffArray.h"
#include "MetricsStatsArray.h"

using namespace std::literals;


namespace MetroCollect {
	class MetricsControllerDelegate;

	class MetricsController {
		public:
			static constexpr bool defaultSendValues = false;
			static constexpr bool defaultSendStats = false;
#if PROFILING
			static constexpr std::chrono::milliseconds defaultSamplingInterval = 0ms;
#else
			static constexpr std::chrono::milliseconds defaultSamplingInterval = 100ms;
#endif
			static constexpr size_t defaultProcessingWindowLength = 10;
			static constexpr size_t defaultProcessingWindowOverlap = 0;
			static constexpr bool defaultConvertToUnitsPerSecond = true;

			struct MetricsStatsIntermediate {
				MetricsStatsArray<size_t> minIndexes;
				MetricsStatsArray<size_t> maxIndexes;
				MetricsStatsArray<DiffValueType> sum;
				MetricsStatsArray<DiffValueType> sumSquared;

				template<typename Function>
				void forEach(Function func) {
					func(minIndexes, Statistics::StatsIndexMin);
					func(maxIndexes, Statistics::StatsIndexMax);
					func(sum, Statistics::StatsIndexIntermediateSum);
					func(sumSquared, Statistics::StatsIndexIntermediateSumSquared);
				}

				template<typename Function>
				void forEach(Function func) const {
					func(minIndexes, Statistics::StatsIndexMin);
					func(maxIndexes, Statistics::StatsIndexMax);
					func(sum, Statistics::StatsIndexIntermediateSum);
					func(sumSquared, Statistics::StatsIndexIntermediateSumSquared);
				}
			};

			struct MetricsStats {
				MetricsStatsArray<DiffValueType> min;
				MetricsStatsArray<DiffValueType> max;
				MetricsStatsArray<DiffValueType> average;
				MetricsStatsArray<DiffValueType> stdDev;

				template<typename Function>
				void forEach(Function func) {
					func(min, Statistics::StatsMin);
					func(max, Statistics::StatsMax);
					func(average, Statistics::StatsAverage);
					func(stdDev, Statistics::StatsStdDev);
				}

				template<typename Function>
				void forEach(Function func) const {
					func(min, Statistics::StatsMin);
					func(max, Statistics::StatsMax);
					func(average, Statistics::StatsAverage);
					func(stdDev, Statistics::StatsStdDev);
				}
			};

		protected:
			MetricsControllerDelegate& delegate_;

			MetricsSource::SourceInterests sourceInterests_;
			std::vector<size_t> requestedMetrics_;
			bool sendValues_;
			bool sendStats_;

			bool isCollecting_;
			std::chrono::milliseconds samplingInterval_;
			double unitsPerSecondFactor_;
			bool convertToUnitsPerSecond_;

			size_t processingWindowLength_;
			size_t processingWindowOverlap_;

			std::unique_ptr<MetricsDataArray> currentMetrics_;
			std::unique_ptr<MetricsDataArray> previousMetrics_;

			CircularArray::CircularArray<MetricsDiffArray> metricsValues_;
			MetricsStatsIntermediate metricsStatsIntermediate_;
			MetricsStats metricsStats_;

			void updateMetrics();
			void updateIterativeStats();
			void computeFinalStats();
			void resetIterativeStats();

		public:
			MetricsController(MetricsControllerDelegate& delegate) noexcept;

			MetricsControllerDelegate& delegate() const noexcept;

			bool isCollecting() const noexcept;

			bool sendValues() const noexcept;
			bool sendStats() const noexcept;
			const std::vector<size_t>& requestedMetrics() const noexcept;
			std::chrono::milliseconds samplingInterval() const noexcept;
			size_t processingWindowLength() const noexcept;
			size_t processingWindowOverlap() const noexcept;
			bool convertToUnitsPerSeconds() const noexcept;

			void setSendValues(bool sendValues) noexcept;
			void setSendStats(bool sendStats) noexcept;
			void setRequestedMetrics(const MetricsArray<Statistics::Stats>& requestedMetrics) noexcept;
			void setSamplingInterval(std::chrono::milliseconds interval) noexcept;
			void setProcessingWindow(size_t length, size_t overlap) noexcept;
			void setConvertToUnitsPerSeconds(bool convertToUnitsPerSecond) noexcept;

			void collectMetrics();

			bool isMetricNull(size_t index) const;
	};


	class MetricsControllerDelegate {
		public:
			virtual void metricsContollerCollectedMetricsValues(const MetricsController& metricsController, const MetricsDiffArray& metricsDiff, const MetricsDataArray& previousMetrics, const MetricsDataArray& currentMetrics) = 0;
			virtual void metricsContollerCollectedMetricsStats(const MetricsController& metricsController, const MetricsController::MetricsStats& metricsStats) = 0;
			virtual bool metricsContollerShouldStopCollectingMetrics(const MetricsController& metricsController) = 0;
	};
}
