//
// MetricTypes.h
//
// Created on July 23th 2018
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

#include <cstdint>
#include <vector>


namespace MetroCollect {
	using DataValueType = int64_t;													//!< Type of fetched raw metrics
	using DiffValueType = double;													//!< Type of metric variation

	namespace MetricsSource {
		/**
		 * @brief Namespace holding MetricsDataArray iterator types for metric sources
		 */
		namespace DataArray {
			using Iterator = std::vector<DataValueType>::iterator;					//!< Iterator type of MetricsDataArray
			using ConstIterator = std::vector<DataValueType>::const_iterator;		//!< Const iterator type of MetricsDataArray
		}

		/**
		 * @brief Namespace holding MetricsDiffArray iterator types for metric sources
		 */
		namespace DiffArray {
			using Iterator = std::vector<DiffValueType>::iterator;					//!< Iterator type of MetricsDiffArray
			using ConstIterator = std::vector<DiffValueType>::const_iterator;		//!< Const iterator type of MetricsDiffArray
		}
	}
}
