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
	/**
	 * @brief Class to store metric statistics, and the indexes of those requested
	 *
	 * @tparam T Type of stored metrics on the container
	 */
	template<typename T>
	class MetricsStatsArray : public MetricsArray<T> {
		public:
			/**
			 * @brief Small struct to store an index of a value in a container and a reference to its value
			 */
			struct IndexedValue {
				size_t index;			//!< Index of the value in the container
				T& value;				//!< Reference to the value

				/**
				 * @brief Construct a new Indexed Value object
				 *
				 * @param anIndex index of the value in the container
				 * @param aValue reference to the value
				 */
				IndexedValue(size_t anIndex, T& aValue) : index(anIndex), value(aValue) { }
			};

		protected:
			std::set<size_t> indexes_;								//!< Set of requested metrics
			std::vector<IndexedValue> values_;						//!< Array of requested metrics

			std::chrono::system_clock::time_point startTime_;		//!< Time of the earlier metric

		public:
			/**
			 * @brief Construct a new Metrics Stats Array object
			 */
			MetricsStatsArray() noexcept;

			/**
			 * @brief Get time of earlier metrics
			 *
			 * @return Time of earlier metrics
			 */
			const std::chrono::system_clock::time_point& startTime() const noexcept;

			/**
			 * @brief Get time of latest metrics
			 *
			 * @return Time of latest metrics
			 */
			const std::chrono::system_clock::time_point& endTime() const noexcept;


			/**
			 * @brief Set the time of earlier metrics used to compute statistics
			 *
			 * @param startTime the time of earlier metrics
			 */
			void setStartTime(std::chrono::system_clock::time_point startTime) noexcept;

			/**
			 * @brief Set the time of latest metrics used to compute statistics
			 *
			 * @param endTime the time of latest metrics
			 */
			void setEndTime(std::chrono::system_clock::time_point endTime) noexcept;


			/**
			 * @brief Returns set of requested metrics
			 *
			 * @return The set of requested metrics
			 */
			const std::set<size_t>& indexes() const noexcept;

			/**
			 * @brief Set which indexes are requested
			 *
			 * @param indexes Set of requested metrics
			 */
			void setIndexes(std::set<size_t> indexes) noexcept;

			/**
			 * @brief Returns the number of requested metrics
			 *
			 * @return the number of requested metrics
			 */
			size_t indexCount() const noexcept;


			/**
			 * @brief Returns the requested metric indexes and values
			 *
			 * @return Array of requested metric indexes and values
			 */
			const std::vector<IndexedValue>& indexedValues() const noexcept;
	};
}
