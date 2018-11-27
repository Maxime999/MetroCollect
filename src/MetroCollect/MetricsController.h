//
// MetricsController.cc
//
// Created on July 24th 2018
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

#include <chrono>
#include <memory>
#include <vector>

#include <CircularArray/CircularArray.h>

#include "MetricsDataArray.h"
#include "MetricsDiffArray.h"
#include "MetricsStatsArray.h"

using namespace std::literals;


namespace MetroCollect {
	class MetricsControllerDelegate;

	/**
	 * @brief Class to control the collection of metrics
	 */
	class MetricsController {
		public:
			static constexpr bool defaultSendValues = false;								//!< Default parameter option
			static constexpr bool defaultSendStats = false;									//!< Default parameter option
#if PROFILING
			static constexpr std::chrono::milliseconds defaultSamplingInterval = 0ms;		//!< Default parameter option
#else
			static constexpr std::chrono::milliseconds defaultSamplingInterval = 100ms;		//!< Default parameter option
#endif
			static constexpr size_t defaultProcessingWindowLength = 10;						//!< Default parameter option
			static constexpr size_t defaultProcessingWindowOverlap = 0;						//!< Default parameter option
			static constexpr bool defaultConvertToUnitsPerSecond = true;					//!< Default parameter option

			/**
			 * @brief Struct to store all intermediate metric stats
			 */
			struct MetricsStatsIntermediate {
				MetricsStatsArray<size_t> minIndexes;				//!< Current indexes of minimums
				MetricsStatsArray<size_t> maxIndexes;				//!< Current indexes of maximums
				MetricsStatsArray<DiffValueType> sum;				//!< Current sum of all values
				MetricsStatsArray<DiffValueType> sumSquared;		//!< Current sum of all values squared

				/**
				 * @brief Execute a function for each intermediate stats array
				 *
				 * @tparam Function Function type, which must accept two arguments: implicitly convertible to the intermediate stats array type and an StatIndex
				 * @param func function to execute
				 */
				template<typename Function>
				void forEach(Function func) {
					func(minIndexes, Statistics::StatsIndexMin);
					func(maxIndexes, Statistics::StatsIndexMax);
					func(sum, Statistics::StatsIndexIntermediateSum);
					func(sumSquared, Statistics::StatsIndexIntermediateSumSquared);
				}

				/**
				 * @brief Execute a const function for each intermediate stats array
				 *
				 * @tparam Function Const function type, which must accept two arguments: implicitly convertible to the intermediate stats array type and an StatIndex
				 * @param func function to execute
				 */
				template<typename Function>
				void forEach(Function func) const {
					func(minIndexes, Statistics::StatsIndexMin);
					func(maxIndexes, Statistics::StatsIndexMax);
					func(sum, Statistics::StatsIndexIntermediateSum);
					func(sumSquared, Statistics::StatsIndexIntermediateSumSquared);
				}
			};

			/**
			 * @brief Struct to store all final metric stats
			 */
			struct MetricsStats {
				MetricsStatsArray<DiffValueType> min;			//!< Array of minimums
				MetricsStatsArray<DiffValueType> max;			//!< Array of maximums
				MetricsStatsArray<DiffValueType> average;		//!< Array of averages
				MetricsStatsArray<DiffValueType> stdDev;		//!< Array of standard deviations

				/**
				 * @brief Execute a function for each stats array
				 *
				 * @tparam Function Function type, which must accept two arguments: implicitly convertible to the stats array type and an StatIndex
				 * @param func function to execute
				 */
				template<typename Function>
				void forEach(Function func) {
					func(min, Statistics::StatsMin);
					func(max, Statistics::StatsMax);
					func(average, Statistics::StatsAverage);
					func(stdDev, Statistics::StatsStdDev);
				}

				/**
				 * @brief Execute a const function for each stats array
				 *
				 * @tparam Function Const function type, which must accept two arguments: implicitly convertible to the stats array type and an StatIndex
				 * @param func function to execute
				 */
				template<typename Function>
				void forEach(Function func) const {
					func(min, Statistics::StatsMin);
					func(max, Statistics::StatsMax);
					func(average, Statistics::StatsAverage);
					func(stdDev, Statistics::StatsStdDev);
				}
			};

		protected:
			MetricsControllerDelegate& delegate_;								//!< Delegate to tell when something happens
			MetricsSource::SourceInterests sourceInterests_;					//!< Boolean arrays to store source interests
			std::vector<size_t> requestedMetrics_;								//!< Indexes of metrics requested to be collected
			bool sendValues_;													//!< Whether to give values to the delegate
			bool sendStats_;													//!< Whether to compute stats and give them to the delegate

			bool isCollecting_;													//!< Whether the receiver is collecting metrics
			std::chrono::milliseconds samplingInterval_;						//!< Metrics sampling interval
			double unitsPerSecondFactor_;										//!< Factor to convert metric differences to units per second
			bool convertToUnitsPerSecond_;										//!< Whether to convert metric differences to units per second

			size_t processingWindowLength_;										//!< Length of processing window used to compute statistics
			size_t processingWindowOverlap_;									//!< Overlap of processing window used to compute statistics

			std::unique_ptr<MetricsDataArray> currentMetrics_;					//!< Pointer to latest metric raw values
			std::unique_ptr<MetricsDataArray> previousMetrics_;					//!< Pointer to earlier metric raw values

			CircularArray::CircularArray<MetricsDiffArray> metricsValues_;		//!< CircularArray holding all metric differences, used to compute statistics
			MetricsStatsIntermediate metricsStatsIntermediate_;					//!< Intermediate statistics arrays
			MetricsStats metricsStats_;											//!< Final statistics arrays

			void updateMetrics();												//!< Fetch latest metrics from source and compute their variations
			void updateIterativeStats();										//!< Update iterative stats with new MetricsDiffArray
			void computeFinalStats();											//!< Compute final stats at the end of processing window
			void resetIterativeStats();											//!< Reset iterative stats to begin new processing window

		public:
			/**
			 * @brief Construct a new Metrics Controller object
			 *
			 * @param delegate The controller's delegate
			 */
			MetricsController(MetricsControllerDelegate& delegate) noexcept;


			/**
			 * @brief Returns the controller's delegate
			 */
			MetricsControllerDelegate& delegate() const noexcept;


			/**
			 * @brief Returns whether the controller is collecting metrics
			 */
			bool isCollecting() const noexcept;

			/**
			 * @brief Returns whether the controller alerts the delegate after collecting new values
			 */
			bool sendValues() const noexcept;

			/**
			 * @brief Returns whether the controller alerts the delegate after computing statistics
			 */
			bool sendStats() const noexcept;

			/**
			 * @brief Returns the list of request metrics
			 */
			const std::vector<size_t>& requestedMetrics() const noexcept;

			/**
			 * @brief Returns the metrics sampling interval
			 */
			std::chrono::milliseconds samplingInterval() const noexcept;

			/**
			 * @brief Returns the length of the processing window used to compute statistics
			 */
			size_t processingWindowLength() const noexcept;

			/**
			 * @brief Returns the overlap of the processing window used to compute statistics
			 */
			size_t processingWindowOverlap() const noexcept;

			/**
			 * @brief Returns whether to convert metric differences to units per second
			 */
			bool convertToUnitsPerSeconds() const noexcept;


			/**
			 * @brief Sets whether the controller alerts the delegate after collecting new values
			 */
			void setSendValues(bool sendValues) noexcept;

			/**
			 * @brief Sets whether the controller alerts the delegate after computing statistics
			 */
			void setSendStats(bool sendStats) noexcept;

			/**
			 * @brief Sets the list of request metrics
			 */
			void setRequestedMetrics(const MetricsArray<Statistics::Stats>& requestedMetrics) noexcept;

			/**
			 * @brief Sets the metrics sampling interval
			 */
			void setSamplingInterval(std::chrono::milliseconds interval) noexcept;

			/**
			 * @brief Sets the processing window parameters used to compute statistics
			 *
			 * @param length length of the processing window
			 * @param overlap overlap of the processing window
			 */
			void setProcessingWindow(size_t length, size_t overlap) noexcept;

			/**
			 * @brief Sets whether to convert metric differences to units per second
			 */
			void setConvertToUnitsPerSeconds(bool convertToUnitsPerSecond) noexcept;


			/**
			 * @brief Launch metric collection loop
			 */
			void collectMetrics();


			/**
			 * @brief Checks wether a metric is zero
			 *
			 * @param index index of the metric to check
			 * @return *true* if the metric earlier and latest values are zero
			 * @return *false* otherwise
			 */
			bool isMetricNull(size_t index) const;
	};


	/**
	 * @brief Abstract delegate of MetricsController
	 *
	 * Each function is called when the MetricsController has done something of importance
	 */
	class MetricsControllerDelegate {
		public:
			/**
			 * @brief Function called when the MetricsController has collected new values
			 *
			 * @param metricsController the calling MetricsController
			 * @param metricsDiff computed metrics differences
			 * @param previousMetrics earlier collected metrics
			 * @param currentMetrics latest collected metrics
			 */
			virtual void metricsContollerCollectedMetricsValues(const MetricsController& metricsController, const MetricsDiffArray& metricsDiff, const MetricsDataArray& previousMetrics, const MetricsDataArray& currentMetrics) = 0;

			/**
			 * @brief Function called when the MetricsController has computed statistics
			 *
			 * @param metricsController the calling MetricsController
			 * @param metricsStats computed statistics
			 */
			virtual void metricsContollerCollectedMetricsStats(const MetricsController& metricsController, const MetricsController::MetricsStats& metricsStats) = 0;

			/**
			 * @brief Function called after each iteration to ask the delegate whether the controller should stop collecting metrics
			 *
			 * @param metricsController the calling MetricsController
			 * @return *true* if the controller should stop
			 * @return *false* otherwise
			 */
			virtual bool metricsContollerShouldStopCollectingMetrics(const MetricsController& metricsController) = 0;
	};
}
