//
// SourceInterests.h
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

#include <memory>
#include <vector>


namespace MetroCollect::MetricsSource {
	/**
	 * @brief Boolean array to keep track of which fields a source has to fetch metrics for
	 *
	 * Interests
	 */
	struct Interests {
		uint64_t value;		//!< Boolean array underlying storage


		/**
		 * @brief Construct a new Interests object (each value defaults to false)
		 *
		 */
		inline Interests() : value(0) { }

		/**
		 * @brief Construct a new Interests object
		 *
		 * @tparam T Integer-like type to read values from
		 * @param v boolean array values to copy
		 */
		template<typename T>
		inline Interests(T v) : value(v) { }


		/**
		 * @brief Assigns values to the boolean array
		 *
		 * @tparam T Integer-like type to read values from
		 * @param v boolean array values to copy
		 * @return `*this`
		 */
		template<typename T>
		inline Interests& operator=(const T v) {
			value = v;
			return *this;
		}


		/**
		 * @brief Checks wether any bit is true
		 *
		 * @return *true* if any bit is true
		 * @return *false* otherwise
		 */
		inline bool any() const {
			return value != 0;
		}


		/**
		 * @brief Checks wether all bit are false
		 *
		 * @return *true* if all bits are false
		 * @return *false* otherwise
		 */
		inline bool none() const {
			return value == 0;
		}


		/**
		 * @brief Checks if a specific bit is true
		 *
		 * @tparam T Integer-like index type
		 * @param index position of the bit to check
		 * @return the specified bit
		 */
		template<typename T>
		inline bool isSet(T index) const {
			return (value & (static_cast<size_t>(1) << (index % (8 * sizeof(size_t))))) != 0;
		}


		/**
		 * @brief Set a specific bit's value to true
		 *
		 * @tparam T Integer-like index type
		 * @param index position of the bit to set to true
		 */
		template<typename T>
		inline void set(T index) {
			value |= (static_cast<size_t>(1) << (index % (8 * sizeof(size_t))));
		}


		/**
		 * @brief Set a specific bit's value to false
		 *
		 * @tparam T Integer-like index type
		 * @param index position of the bit to set to false
		 */
		template<typename T>
		inline void unSet(T index) {
			value &= ~(static_cast<size_t>(1) << (index % (8 * sizeof(size_t))));
		}
	};


	/**
	 * @brief Construct a new Interests object
	 *
	 * @param v default value to copy to all bits
	 */
	template<>
	inline Interests::Interests(bool v) : value(v ? ~0 : 0) { }


	/**
	 * @brief Assigns values to the boolean array
	 *
	 * @param v boolean value to copy to each bit
	 * @return `*this`
	 */
	template<>
	inline Interests& Interests::operator=(const bool v) {
		value = (v ? ~0 : 0);
		return *this;
	}


	using SourceInterests = std::shared_ptr<std::vector<MetricsSource::Interests>>;		//!< Type used to store and share interests of multiple sources

	/**
	 * @brief Construct a new shared pointer to a SourceInterests object
	 *
	 * @param value the default source interests value
	 * @return the newly built shared pointer
	 */
	SourceInterests makeSourceInterests(bool value = false);
}
