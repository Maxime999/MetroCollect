//
// MetricsStatsArray.cc
//
// Created on July 25th 2018
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

#include "MetricsStatsArray.h"


namespace MetroCollect {
	template<typename T>
	MetricsStatsArray<T>::MetricsStatsArray() noexcept : MetricsArray<T>() { }


	template<typename T>
	const std::chrono::system_clock::time_point& MetricsStatsArray<T>::startTime() const noexcept {
		return this->startTime_;
	}

	template<typename T>
	const std::chrono::system_clock::time_point& MetricsStatsArray<T>::endTime() const noexcept {
		return this->timestamp_;
	}


	template<typename T>
	void MetricsStatsArray<T>::setStartTime(std::chrono::system_clock::time_point startTime) noexcept {
		this->startTime_ = std::move(startTime);
	}

	template<typename T>
	void MetricsStatsArray<T>::setEndTime(std::chrono::system_clock::time_point endTime) noexcept {
		this->timestamp_ = std::move(endTime);
	}


	template<typename T>
	const std::set<size_t>& MetricsStatsArray<T>::indexes() const noexcept {
		return this->indexes_;
	}

	template<typename T>
	void MetricsStatsArray<T>::setIndexes(std::set<size_t> indexes) noexcept {
		this->indexes_ = std::move(indexes);
		this->values_.clear();

		for (size_t i : this->indexes_)
			this->values_.emplace_back(i, this->data_[i]);
	}


	template<typename T>
	size_t MetricsStatsArray<T>::indexCount() const noexcept {
		return this->indexes_.size();
	}


	template<typename T>
	const std::vector<typename MetricsStatsArray<T>::IndexedValue>& MetricsStatsArray<T>::indexedValues() const noexcept {
		return this->values_;
	}


	template class MetricsStatsArray<size_t>;
	template class MetricsStatsArray<DiffValueType>;
}
