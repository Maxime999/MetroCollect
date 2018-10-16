//
// MetricsSourceArray.h
//
// Created on July 18th 2018
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

#include <string>
#include <string_view>
#include <vector>

#include "MetricTypes.h"
#include "SourceField.h"
#include "SourceInterests.h"
#include "SourceTools.h"


/**
 * @brief Namespace for sources of metrics objects and operations
 */
namespace MetroCollect::MetricsSource {
	/**
	 * @brief Singleton abstract class, base class of all metric sources classes.
	 *
	 * A subclass should override all methods of this class.
	 */
	class SourceBase {
		protected:
			SourceBase() {}			//!< Protected default constructor
			~SourceBase() {}		//!< Protected default destructor

		public:
			SourceBase(const SourceBase&) = delete;			//!< Deleted copy constructor
			void operator=(const SourceBase&) = delete;		//!< Deleted copy operator


			/**
			 * @brief Get the number of field the source has
			 *
			 * @return Number of fields of the source
			 */
			virtual size_t fieldCount() const noexcept = 0;

			/**
			 * @brief Search for fields associated to a specific name
			 *
			 * @param fieldName the name of the fields to search
			 * @param interests if the pointer is not `nullptr`, the interest bit corresponding to the fields will be set
			 * @return the indexes associated with the field name
			 */
			virtual const std::vector<size_t> indexesOfFieldName(const FieldName& fieldName, Interests* interests = nullptr) const noexcept = 0;

			/**
			 * @brief Get the prefix by which all field names should begin with
			 *
			 * @return the source's names prefix
			 */
			virtual const std::string fieldNameSourcePrefix() const noexcept = 0;

			/**
			 * @brief Get details about a specific field
			 *
			 * @param index index of the field to get details of
			 * @return details of the field
			 */
			virtual const FieldInfo fieldInfoAtIndex(size_t index) const noexcept = 0;

			/**
			 * @brief Get all fields details, field which share a common name should appear only once
			 *
			 * @return all fields details
			 */
			virtual const std::vector<FieldInfo> allFieldsInfo() const noexcept = 0;


			/**
			 * @brief Fetch the latest metrics
			 *
			 * @param interests boolean array to know for which fields metrics should be fetched
			 * @param current iterator to write metrics to
			 */
			virtual void fetchData(const Interests& interests, DataArray::Iterator current) = 0;


			/**
			 * @brief Compute the variation (in appropriate unit) of metrics
			 *
			 * @param interests interests boolean array to know for which fields metrics variation should be computed
			 * @param diff iterator to write metrics variation to
			 * @param current iterator to read latest metrics from
			 * @param previous iterator to read previous metrics from
			 * @param factor factor to convert to unit per second, if relevant
			 */
			virtual void computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor = 1) noexcept = 0;
	};
}
