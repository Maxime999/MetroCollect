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
	class SourceProcStat : public SourceBase {
		protected:
			static constexpr std::string_view filePath = "/proc/stat"sv;

			static constexpr std::string_view sourcePrefix = "cpu"sv;
			static constexpr std::string_view fieldNameOtherPrefix = "other"sv;
			static constexpr std::string_view fieldNameAllCoresPrefix = "all"sv;
			static constexpr std::string_view fieldNameCorePrefix = "core"sv;
			static constexpr std::string_view fieldNameCoreDescription = "cpuID"sv;
			static constexpr std::string_view fieldNameCoreAll = "*"sv;
			static constexpr std::string_view cpuField = "cpu"sv;
			static constexpr size_t cpuFieldCount = 10;
			static constexpr std::array<std::string_view, cpuFieldCount> cpuFieldNames = {"user"sv, "nice"sv, "system"sv, "idle"sv, "iowait"sv, "irq"sv, "softirq"sv, "steal"sv, "guest"sv, "guestNice"sv};
			static constexpr std::string_view percentUnit = "%"sv;
			static constexpr std::string_view defaultUnit = ""sv;

			size_t cpuCount_;
			std::vector<std::string> otherFieldNames_;
			std::ifstream file_;
			std::vector<char> buffer_;

			SourceProcStat();
			void parseFields();

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
