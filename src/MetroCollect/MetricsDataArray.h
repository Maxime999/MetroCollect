//
// MetricsDataArray.h
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

#include "MetricsArray.h"


namespace MetroCollect {
	/**
	 * @brief Class to fetch metrics from the kernel and store them
	 */
	class MetricsDataArray : public MetricsArray<DataValueType> {
		public:
			/**
			 * @brief Construct a new Metrics Data Array object
			 */
			MetricsDataArray() noexcept;

			/**
			 * @brief Construct a new Metrics Data Array object
			 *
			 * @param sourceInterests boolean arrays to store source interests
			 */
			explicit MetricsDataArray(MetricsSource::SourceInterests sourceInterests) noexcept;


			/**
			 * @brief Fetch new metric values from each source
			 *
			 * Metric values are stored directly into the underlying array
			 */
			void updateData();
	};
}
