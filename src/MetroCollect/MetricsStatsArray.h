//
// MetricsStatsArray.h
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

#pragma once

#include <set>

#include "MetricsArray.h"
#include "Statistics.h"


namespace MetroCollect {
	template<typename T>
	class MetricsStatsArray : public MetricsArray<T> {
		public:
			struct IndexedValue {
				size_t index;
				T& value;

				IndexedValue(size_t anIndex, T& aValue) : index(anIndex), value(aValue) { }
			};

		protected:
			std::set<size_t> indexes_;
			std::vector<IndexedValue> values_;

			std::chrono::system_clock::time_point startTime_;

		public:
			MetricsStatsArray() noexcept;

			const std::chrono::system_clock::time_point& startTime() const noexcept;
			const std::chrono::system_clock::time_point& endTime() const noexcept;

			void setStartTime(std::chrono::system_clock::time_point startTime) noexcept;
			void setEndTime(std::chrono::system_clock::time_point endTime) noexcept;

			const std::set<size_t>& indexes() const noexcept;
			void setIndexes(std::set<size_t> indexes) noexcept;

			size_t indexCount() const noexcept;

			const std::vector<IndexedValue>& indexedValues() const noexcept;
	};
}
