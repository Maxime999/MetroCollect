//
// SourceTools.h
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

#include <array>
#include <cctype>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;


namespace MetroCollect::MetricsSource {
	/**
	 * @brief Fast and unsafe unsigned integer parser
	 *
	 * @param buffer reference to a character string buffer with an unsigned integer to parse
	 * @return the parsed value
	 */
	inline uint64_t parseUint(const char*& buffer) noexcept {
		uint64_t result = 0;
		while (!std::isdigit(*buffer))
			buffer++;
		while (std::isdigit(*buffer)) {
			result = (result << 1) + (result << 3) + *buffer - '0';
			buffer++;
		}
		return result;
	}

	/**
	 * @brief Attempts to open a file and set buffer size accordingly
	 *
	 * @param file file to open
	 * @param buffer buffer to read file into
	 * @param path file path to open
	 * @return *true* if file was opened correctly
	 * @return *false* otherwise
	 */
	bool resetFile(std::ifstream& file, std::vector<char>& buffer, const std::string_view& path);

	/**
	 * @brief Attempts to read file into buffer
	 *
	 * @param file file to open
	 * @param buffer buffer to read file into
	 * @param path file path to read
	 * @return *true* if file was read correctly
	 * @return *false* otherwise
	 */
	bool readFile(std::ifstream& file, std::vector<char>& buffer, const std::string_view& path);


	/**
	 * @brief Stuct to associate a field name containing a certain key with a unit
	 *
	 */
	struct KeyUnit {
		std::string_view key;		//<! Key to find in field name for the unit to be applicable
		std::string_view unit;		//<! The unit

		constexpr KeyUnit() : key(""sv), unit(""sv) { }														//<! Default constructor
		constexpr KeyUnit(std::string_view aKey, std::string_view aUnit) : key(aKey), unit(aUnit) { }		//<! Constructor with values
	};

	/**
	 * @brief Tries to associate a field name with a unit, falling back on the default unit if none matches.
	 *
	 * @tparam N Size of the key/unit association array
	 * @param name field name to find a unit for
	 * @param keyUnitAssociation array of keys and unit asociation. Keys are searched in the field name, the first match determines the key
	 * @param defaultUnit default unit to use if no key matches
	 * @return the best match unit
	 */
	template<size_t N>
	std::string findUnit(const std::string& name, const std::array<KeyUnit, N>& keyUnitAssociation, const std::string_view& defaultUnit) {
		for (const auto& val : keyUnitAssociation) {
			if (name.find(val.key) != std::string::npos)
				return std::string(val.unit);
		}
		return std::string(defaultUnit);
	}


	/**
	 * @brief Alphanumeric string comparison
	 *
	 * Inspired from http://www.davekoelle.com/alphanum.html
	 *
	 * @param l pointer to first character string
	 * @param r pointer to second character string
	 * @return *negative* value if `l` appears before `r`, in alphanumerical order
	 * @return *zero* if both character sequences compare equivalent
	 * @return *positive* value if `l` appears after `r`, in alphanumerical order
	 *
	 ---
	 *
	 * **License**
	 *
	 * Released under the MIT License - https://opensource.org/licenses/MIT
	 *
	 * Permission is hereby granted, free of charge, to any person obtaining
	 * a copy of this software and associated documentation files (the "Software"),
	 * to deal in the Software without restriction, including without limitation
	 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
	 * and/or sell copies of the Software, and to permit persons to whom the
	 * Software is furnished to do so, subject to the following conditions:
	 *
	 * The above copyright notice and this permission notice shall be included
	 * in all copies or substantial portions of the Software.
	 *
	 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
	 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
	 * USE OR OTHER DEALINGS IN THE SOFTWARE.
	 */
	int alphanumCompare(const char* l, const char* r);
}
