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

	/**
	 * @brief Generic class to store and manage metrics
	 *
	 * @tparam T Type of stored metrics on the container
	 */
	template<typename T>
	class MetricsArray {
		public:
			using ValueType = T;												//!< Type of stored metrics on the container
			using Iterator = typename std::vector<T>::iterator;					//!< Iterator type
			using ConstIterator = typename std::vector<T>::const_iterator;		//!< Const iterator type

		protected:
			MetricsSource::SourceInterests sourceInterests_;					//!< Boolean arrays to store source interests
			std::chrono::system_clock::time_point timestamp_;					//!< Time at which metrics were collected

			std::vector<T> data_;												//!< Underlying data storage
			MetricsSource::MetricsSourceArray sources_;							//!< Array of sources to fetch data from
			std::array<std::pair<Iterator, Iterator>, MetricsSource::MetricsSourceArray::count> sourceIterators_;		//!< Iterators pairs associating metrics to a source

			friend MetricsDiffArray;											//!< To allow internal access by subclass

			void setSize() noexcept;											//!< Resize the underlying container to fit the total number of metrics
			void setIterators() noexcept;										//!< Update internal iterators pairs

		public:
			/**
			 * @brief Construct a new Metrics Array object
			 */
			MetricsArray() noexcept;

			/**
			 * @brief Construct a new Metrics Array object
			 *
			 * @param sourceInterests boolean arrays to store source interests
			 */
			explicit MetricsArray(MetricsSource::SourceInterests sourceInterests) noexcept;


			/**
			 * @brief Copy constructor
			 *
			 * @param other Metrics Array to copy from
			 */
			MetricsArray(const MetricsArray& other) noexcept;

			/**
			 * @brief Assignment operator
			 *
			 * @param other Metrics Array to copy from
			 * @return *this* with updated values
			 */
			MetricsArray& operator=(const MetricsArray& other) noexcept;


			/**
			 * @brief Returns the source interests
			 *
			 * @return The source interests
			 */
			MetricsSource::SourceInterests sourceInterests() const noexcept;

			/**
			 * @brief Set the source interests
			 *
			 * @param sourceInterests new source interests to copy
			 */
			void setSourcesInterests(MetricsSource::SourceInterests sourceInterests) noexcept;


			/**
			 * @brief Returns the timestamp of stored metrics
			 *
			 * @return The timestamp of stored metrics
			 */
			const std::chrono::system_clock::time_point& timestamp() const noexcept;


			/**
			 * @brief Returns the total number of available metrics.
			 *
			 * It is the same as the size of the underlying array
			 *
			 * @return The total number of available metrics
			 */
			size_t fieldCount() const noexcept;

			/**
			 * @brief Finds indexes of a field from its name across sources
			 *
			 * @param fieldName name of the field to search
			 * @param setInterest whether to activate the corresponding source interests
			 * @return const std::vector<size_t> indexes of fields with given name
			 */
			const std::vector<size_t> indexesOfFieldName(const MetricsSource::FieldName& fieldName, bool setInterest = true) const noexcept;

			/**
			 * @brief Get details about a specific metric field
			 *
			 * @param index Index of field to get details of
			 * @return Details of the field
			 */
			const MetricsSource::FieldInfo fieldInfoAtIndex(size_t index) const noexcept;

			/**
			 * @brief Get details about all fields
			 *
			 * @return Details of all fields
			 */
			const std::vector<MetricsSource::FieldInfo> allFieldsInfo() const noexcept;


			/**
			 * @brief Returns the size of the underlying array.
			 *
			 * It returns the same value as fieldCount().
			 *
			 * @return the size of the underlying array
			 */
			size_t size() const noexcept;

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Reference to the requested element
			 */
			T& at(size_t index);

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Const reference to the requested element
			 */
			const T& at(size_t index) const;

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Reference to the requested element
			 */
			inline T& operator[](size_t index) { return this->data_[index]; }

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Const reference to the requested element
			 */
			inline const T& operator[](size_t index) const { return this->data_[index]; }


			/**
			 * @brief Returns the iterator pair associated with a source
			 *
			 * @param sourceIndex index of the source to get metric iterators
			 * @return Iterator pair (begin, end) of metrics given by the specified source
			 */
			std::pair<Iterator, Iterator> metricsSliceForSource(size_t sourceIndex);


			/**
			 * @brief Returns an iterator to the first element of the container
			 *
			 * @return Iterator to the first element
			 */
			Iterator begin() noexcept;

			/**
			 * @brief Returns an iterator to the first element of the container
			 *
			 * @return Const iterator to the first element
			 */
			const ConstIterator begin() const noexcept;

			/**
			 * @brief Returns an iterator to the element following the last element of the container
			 *
			 * @return Iterator to the element following the last element
			 */
			Iterator end() noexcept;

			/**
			 * @brief Returns an iterator to the element following the last element of the container
			 *
			 * @return Const iterator to the element following the last element
			 */
			const ConstIterator end() const noexcept;
	};
}
