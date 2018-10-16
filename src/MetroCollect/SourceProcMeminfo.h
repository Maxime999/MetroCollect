//
// SourceProcMeminfo.h
//
// Created on August 23rd 2018
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
	class SourceProcMeminfo : public SourceBase {
		protected:
			static constexpr std::string_view filePath = "/proc/meminfo"sv;

			static constexpr std::string_view sourcePrefix = "memory"sv;
			static constexpr std::array fieldUnitsAssociation = { KeyUnit{"huge_pages_"sv, ""sv} };
			static constexpr std::string_view defaultUnit = "bytes"sv;

			std::ifstream file_;
			std::vector<char> buffer_;
			std::vector<std::string> fieldNames_;

			SourceProcMeminfo();
			void parseFields();

		public:
			/**
			 * @brief Get the Instance object
			 */
			static SourceProcMeminfo& get() {
				static SourceProcMeminfo instance;
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
