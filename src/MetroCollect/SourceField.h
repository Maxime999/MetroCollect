//
// SourceField.h
//
// Created on August 27th 2018
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
#include <vector>


namespace MetroCollect::MetricsSource {
	/**
	 * @brief Type used for field names (an array of strings)
	 */
	using FieldName = std::vector<std::string>;


	/**
	 * @brief Object used to describe a field (or metric)
	 */
	struct FieldInfo {
		struct IndexAndDescription {
			size_t index;
			std::string description;

			IndexAndDescription(size_t anIndex, std::string aDescription) : index(anIndex), description(std::move(aDescription)) { }
		};

		FieldName name;														//!< Name of the field
		std::string description;											//!< Description of the field
		std::string unit;													//!< Unit of the field's metric
		std::vector<IndexAndDescription> dynamicIndexes;					//!< Indexes and descriptions of the dynamic parts of the field name


		/**
		 * @brief Construct a new Field Info object
		 */
		FieldInfo() { }

		/**
		 * @brief Construct a new Field Info object
		 *
		 * @param aName name of the field
		 * @param aDescription description of the field
		 * @param aUnit unit of the field
		 */
		FieldInfo(FieldName aName, std::string aDescription, std::string aUnit) :
			name(aName), description(aDescription), unit(aUnit) { }

		/**
		 * @brief Construct a new Field Info object
		 *
		 * @param aName name of the field
		 * @param aDescription description of the field
		 * @param aUnit unit of the field
		 * @param anIndex index of the dynamic part in the field name
		 * @param aDynamicDescription description of the dynamic field name part
		 */
		FieldInfo(FieldName aName, std::string aDescription, std::string aUnit, size_t anIndex, std::string aDynamicDescription) :
			name(aName), description(aDescription), unit(aUnit), dynamicIndexes({ IndexAndDescription{anIndex, std::move(aDynamicDescription)} }) { }


		/**
		 * @brief Checks if the field name contains any dynamic part
		 *
		 * @return *true* if the field name has at least one dynamic part
		 * @return *false* otherwise
		 */
		bool isDynamic() {
			return dynamicIndexes.size() == 0;
		}
	};
}
