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

#include <array>
#include <string_view>

using namespace std::literals;


namespace MetroCollect {
	class MetricsController;

	/**
	 * @brief Class holding various statistics-related types and variables
	 */
	class Statistics {
		friend MetricsController;								//!< MetricsController can access to protected types and variables
		protected:
			/**
			 * @brief Associates a unique index to each statistic
			 *
			 * There are two kinds of statistics:
			 * - public statistics that can be computed and published as metrics
			 * - private intermediate statistics that are only used to compute other statistics
			 */
			enum StatsIndex : uint8_t {
				StatsIndexMin		= 0,						//!< Minimum
				StatsIndexMax		= 1,						//!< Maximum
				StatsIndexAverage	= 2,						//!< Average
				StatsIndexStdDev	= 3,						//!< Standard deviation

				// Private intermediate statistics
				StatsIndexIntermediateSum			= 4,		//!< Sum of value (intermediate statistic)
				StatsIndexIntermediateSumSquared	= 5,		//!< Sum of squares of values (intermediate statistic)
			};

		public:
			static constexpr size_t count = 4;																				//!< Number of available statistics
			static constexpr std::array<std::string_view, count> names = {"min"sv, "max"sv, "average"sv, "stdDev"sv};		//!< Names of available statistics
			static constexpr std::string_view nameStatsAll = "*"sv;

			/**
			 * @brief Bitwise type te select statistics
			 */
			enum Stats : uint64_t {
				StatsMin		= 1 << StatsIndexMin,										//!< Minimum
				StatsMax		= 1 << StatsIndexMax,										//!< Maximum
				StatsAverage	= 1 << StatsIndexAverage,									//!< Average
				StatsStdDev		= 1 << StatsIndexStdDev,									//!< Standard deviation

				StatsNone		= 0,														//!< No statistics selected
				StatsAll		= StatsMin | StatsMax | StatsAverage | StatsStdDev,			//!< All statistics selected
			};
			static constexpr std::array<Stats, count> allStats = {StatsMin, StatsMax, StatsAverage, StatsStdDev};			//!< Array of all available statistics

			static Stats statsFromName(const std::string_view& statName);					//!< Convert a stat name to a stat value
			static Stats statsRequiredForStat(Stats stat);									//!< Returns the stats which are required to compute this stat

		protected:
			static constexpr size_t extraCount = count + 2;									//!< Number of statistics and intermediate values

			/**
			 * @brief Dependency tree of each statistic
			 */
			enum StatsDependency : uint64_t {
				StatsDependencyMin			= StatsMin,										//!< Statistics which require to compute minimum
				StatsDependencyMax			= StatsMax,										//!< Statistics which require to compute maximum
				StatsDependencyStdDev		= StatsStdDev,									//!< Statistics which require to compute standard deviation
				StatsDependencyAverage		= StatsAverage | StatsDependencyStdDev,			//!< Statistics which require to compute average
				StatsDependencySum			= StatsDependencyAverage,						//!< Statistics which require to compute sum
				StatsDependencySumSquared	= StatsDependencyStdDev,						//!< Statistics which require to compute sum of squares
			};
			static constexpr std::array<StatsDependency, extraCount> StatsDependencies = {StatsDependencyMin, StatsDependencyMax, StatsDependencyAverage, StatsDependencyStdDev, StatsDependencySum, StatsDependencySumSquared};		//!< Array of all statistics dependencies
	};


	/**
	 * @brief Performs bitwise OR of two statistics
	 *
	 * @tparam T Type of second operand
	 * @param a values to compute bitwise OR of
	 * @param b values to compute bitwise OR of
	 * @return the result of `a | b`
	 */
	template<typename T>
	constexpr inline Statistics::Stats operator| (Statistics::Stats a, T b) {
		return static_cast<Statistics::Stats>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
	}

	/**
	 * @brief Performs and assigns bitwise OR of two statistics
	 *
	 * @tparam T Type of second operand
	 * @param a values to compute bitwise OR of
	 * @param b values to compute bitwise OR of
	 * @return `a`, with the result of `a | b`
	 */
	template<typename T>
	constexpr inline Statistics::Stats& operator|= (Statistics::Stats& a, T b) {
		a = a | b;
		return a;
	}
}
