//
// SourceProcStat.h
//
// Created on July 13th 2018
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
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "Source.h"

using namespace std::literals;


namespace MetroCollect::MetricsSource {
	/**
	 * @brief Sources of CPU metrics extracted from the file /proc/stat
	 */
	class SourceProcStat : public SourceBase {
		protected:
			static constexpr std::string_view filePath = "/proc/stat"sv;					//!< CPU metrics file path

			static constexpr std::string_view sourcePrefix = "cpu"sv;						//!< Metrics name source prefix
			static constexpr std::string_view fieldNameOtherPrefix = "other"sv;				//!< Metrics name category
			static constexpr std::string_view fieldNameAllCoresPrefix = "all"sv;			//!< Metrics name category
			static constexpr std::string_view fieldNameCorePrefix = "core"sv;				//!< Metrics name category
			static constexpr std::string_view fieldNameCoreDescription = "cpuID"sv;			//!< Metrics name category description
			static constexpr std::string_view fieldNameCoreAll = "*"sv;						//!< Metrics name wildcard
			static constexpr std::string_view cpuField = "cpu"sv;							//!< Metrics name category
			static constexpr size_t cpuFieldCount = 10;										//!< Number of fields for each CPU
			static constexpr std::array<std::string_view, cpuFieldCount> cpuFieldNames = {"user"sv, "nice"sv, "system"sv, "idle"sv, "iowait"sv, "irq"sv, "softirq"sv, "steal"sv, "guest"sv, "guestNice"sv};		//!< CPU states names
			static constexpr std::string_view percentUnit = "%"sv;							//!< Metric unit
			static constexpr std::string_view defaultUnit = ""sv;							//!< Metric unit

			size_t cpuCount_;									//!< Number of CPUs
			std::vector<std::string> otherFieldNames_;			//!< Names of other CPU metrics
			std::ifstream file_;								//!< File descriptor
			std::vector<char> buffer_;							//!< Buffer to put file contents into

			SourceProcStat();									//!< Private default constructor
			void parseFields();									//!< File parser function

		public:
			/**
			 * @brief Get the Instance object
			 */
			static SourceProcStat& get() {
				static SourceProcStat instance;
				return instance;
			}

			size_t fieldCount() const noexcept override final;
			const std::vector<size_t> indexesOfFieldName(const FieldName& fieldName, Interests* interests = nullptr) const noexcept override final;
			const std::string fieldNameSourcePrefix() const noexcept override final;
			const FieldInfo fieldInfoAtIndex(size_t index) const noexcept override final;
			const std::vector<FieldInfo> allFieldsInfo() const noexcept override final;

			void fetchData(const Interests& interests, DataArray::Iterator current) override final;
			void computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor = 1) noexcept override final;
	};
}
