//
// MetricsArray.h
//
// Created on July 23rd 2018
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

#include <array>
#include <chrono>
#include <utility>
#include <vector>

#include "MetricsSourceArray.h"


namespace MetroCollect {
	class MetricsDiffArray;

	template<typename T>
	class MetricsArray {
		public:
			using ValueType = T;
			using Iterator = typename std::vector<T>::iterator;
			using ConstIterator = typename std::vector<T>::const_iterator;

		protected:
			MetricsSource::SourceInterests sourceInterests_;
			std::chrono::system_clock::time_point timestamp_;

			std::vector<T> data_;
			MetricsSource::MetricsSourceArray sources_;
			std::array<std::pair<Iterator, Iterator>, MetricsSource::MetricsSourceArray::count> sourceIterators_;

			friend MetricsDiffArray;

			void setSize() noexcept;
			void setIterators() noexcept;

		public:
			MetricsArray() noexcept;
			explicit MetricsArray(MetricsSource::SourceInterests sourceInterests) noexcept;

			MetricsArray(const MetricsArray& other) noexcept;
			MetricsArray& operator=(const MetricsArray& other) noexcept;

			MetricsSource::SourceInterests sourceInterests() const noexcept;
			void setSourcesInterests(MetricsSource::SourceInterests sourceInterests) noexcept;

			const std::chrono::system_clock::time_point& timestamp() const noexcept;

			size_t fieldCount() const noexcept;
			const std::vector<size_t> indexesOfFieldName(const MetricsSource::FieldName& fieldName, bool setInterest = true) const noexcept;
			const MetricsSource::FieldInfo fieldInfoAtIndex(size_t index) const noexcept;
			const std::vector<MetricsSource::FieldInfo> allFieldsInfo() const noexcept;

			size_t size() const noexcept;
			T& at(size_t index);
			const T& at(size_t index) const;
			inline T& operator[](size_t index) { return this->data_[index]; }
			inline const T& operator[](size_t index) const { return this->data_[index]; }

			std::pair<Iterator, Iterator> metricsSliceForSource(size_t sourceIndex);

			Iterator begin() noexcept;
			const ConstIterator begin() const noexcept;
			Iterator end() noexcept;
			const ConstIterator end() const noexcept;
	};
}
