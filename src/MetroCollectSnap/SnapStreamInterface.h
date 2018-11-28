//
// SnapStreamInterface.h
//
// Created on July 27th 2018
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

#include <functional>
#include <array>
#include <chrono>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <MetroCollect/MetricsController.h>
#include <snap/plugin.h>

using namespace std::literals;


namespace SnapInterface {
	/**
	 * @brief Interface class between MetricsController and Snap's streaming-collector plugin class
	 */
	class SnapStreamInterface final : public Plugin::StreamCollectorInterface, public MetroCollect::MetricsControllerDelegate {
		public:
			static constexpr int appVersion = 2;								//!< Snap plugin version
			static constexpr std::string_view appName = "metrocollect"sv;		//!< Snap plugin name

			/**
			 * @brief Strcut to store Snap metric objects and metric data to make it faster to send them to Snap
			 */
			struct MetricsPackage {
				/**
				 * @brief Simple array of metrics with a timestamp
				 */
				struct TimedMetrics {
					std::chrono::system_clock::time_point timestamp;			//!< Timestamp of the metrics
					std::vector<MetroCollect::DiffValueType> values;			//!< Array of metrics
				};

				std::vector<Plugin::Metric> computedMetrics;					//!< Cache for Snap metrics
				std::vector<Plugin::Metric*> metricsToSend;						//!< Array of pointers to Snap metrics to be sent
				std::unique_ptr<TimedMetrics> currentMetrics;					//!< Earlier values that may be sent
				std::unique_ptr<TimedMetrics> nextMetrics;						//!< Latest values kept to be sent on the next iteration
				std::vector<size_t> metricsTimeout;								//!< Sub-sampling counters for constant metrics
				std::vector<size_t*> metricsTimeoutPointer;						//!< Array to match metrics to their sub-sampling counter (one is shared for all statistics)
				std::vector<bool> secondaryMetrics;								//!< Array to mark the most important metric stat (average)

				/**
				 * @brief Reinitializes the receiver and allocates space for metrics
				 *
				 * @param metricsCount The total number of metrics that can be sent
				 */
				void clear(size_t metricsCount) {
					this->computedMetrics.clear();
					this->metricsToSend.clear();
					this->currentMetrics = nullptr;
					this->nextMetrics = nullptr;
					this->metricsTimeout.clear();
					this->metricsTimeout.resize(metricsCount, ((size_t)-1));
					this->metricsTimeoutPointer.clear();
					this->secondaryMetrics.clear();
				}
			};

		protected:
			static constexpr std::array appPrefix = {"cfm"sv/*, "metrocollect"sv*/};								//!< Snap metric name prefix
			static constexpr std::string_view configKeySendValues = "SendValues"sv;									//!< Snap plugin configuration key
			static constexpr std::string_view configKeySendStats = "SendStats"sv;									//!< Snap plugin configuration key
			static constexpr std::string_view configKeySamplingInterval = "SamplingInterval"sv;						//!< Snap plugin configuration key
			static constexpr std::string_view configKeyProcessingWindowLength = "ProcessingWindowLength"sv;			//!< Snap plugin configuration key
			static constexpr std::string_view configKeyProcessingWindowOverlap = "ProcessingWindowOverlap"sv;		//!< Snap plugin configuration key
			static constexpr std::string_view configKeyConvertToUnitsPerSecond = "ConvertToUnitsPerSecond"sv;		//!< Snap plugin configuration key
			static constexpr std::string_view configKeyUnchangedMetricTimeout = "UnchangedMetricTimeout"sv;			//!< Snap plugin configuration key
			static constexpr std::string_view configKeyMaxCollectDuration = "MaxCollectDuration"sv;					//!< Snap plugin configuration key
			static constexpr std::string_view configKeyMaxMetricsBuffer = "MaxMetricsBuffer"sv;						//!< Snap plugin configuration key
			static constexpr std::array configKeysInt = {configKeySamplingInterval, configKeyProcessingWindowLength, configKeyProcessingWindowOverlap, configKeyUnchangedMetricTimeout, configKeyMaxCollectDuration, configKeyMaxMetricsBuffer};		//!< Array of integer-valued configuration keys
			static constexpr std::array configKeysBool = {configKeySendValues, configKeySendStats, configKeyConvertToUnitsPerSecond};		//!< Array of boolean-valued configuration keys
			static constexpr bool defaultSendValues = false;														//!< Snap plugin configuration default value
			static constexpr bool defaultSendStats = true;															//!< Snap plugin configuration default value
			static constexpr size_t defaultUnchangedMetricTimeout = 300;											//!< Snap plugin configuration default value
			static constexpr std::chrono::seconds defaultMaxCollectDuration = 0s;									//!< Snap plugin configuration default value
			static constexpr size_t defaultMaxMetricsBuffer = 0;													//!< Snap plugin configuration default value
			static constexpr std::array<int, configKeysInt.size()> configValuesInt = {MetroCollect::MetricsController::defaultSamplingInterval.count(), MetroCollect::MetricsController::defaultProcessingWindowLength, MetroCollect::MetricsController::defaultProcessingWindowOverlap, SnapStreamInterface::defaultUnchangedMetricTimeout, SnapStreamInterface::defaultMaxCollectDuration.count(), SnapStreamInterface::defaultMaxMetricsBuffer};		//!< Array of integer-valued configuration default values
			static constexpr std::array<bool, configKeysBool.size()> configValuesBool = {defaultSendValues, defaultSendStats, MetroCollect::MetricsController::defaultConvertToUnitsPerSecond};		//!< Array of boolean-valued configuration default values
			static constexpr std::string_view statNamespaceLastComponent = "value"sv;								//!< Snap metric name category
			static constexpr std::string_view statNamespaceDescription = "statistic"sv;								//!< Snap metric name description

			MetroCollect::MetricsController metricsController_;														//!< MetricsController used to collect statistics

			MetroCollect::MetricsArray<MetroCollect::Statistics::Stats> requestedMetrics_;							//!< Array of requested metrics
			size_t unchangedMetricsTimeout_;																		//!< Sub-sampling period for constant metrics

			MetricsPackage valuesPackage_;																			//!< MetricsPackage for metrics from raw counters variations
			MetricsPackage statsPackage_;																			//!< MetricsPackage for metrics from computed statistics


			/**
			 * @brief Configures the plugin and the controller accordingly, unspecified values are reset to default
			 *
			 * @param cfg configuration options to use
			 */
			void setConfig(const Plugin::Config& cfg);


			/**
			 * @brief Generate requestedMetrics array from list of Snap metrics
			 *
			 * @param metrics Snap metrics requested by the user
			 */
			void parseSnapMetrics(const std::vector<Plugin::Metric>& metrics);


			/**
			 * @brief Inserts plugin name prefix into Snap namespace
			 *
			 * @param ns Snap namespace to modify
			 */
			void insertAppPrefixToNamespace(std::vector<std::string>& ns);

			/**
			 * @brief Configures a MetricsPackage object according to requested metrics
			 *
			 * @param package package to configure
			 * @param suffix suffix to add to all package's metrics names (generally a statistic name)
			 * @param condition boolean function specifying whether a requested metric should be included, and whether it is a primary metric
			 */
			void createRequestedMetrics(MetricsPackage& package, std::string suffix, std::function<bool(MetroCollect::Statistics::Stats, bool*)> condition);

			/**
			 * @brief Initializes a MetricsPackage sub-sampling counters
			 *
			 * @param package
			 */
			void fillMetricsPackage(MetricsPackage& package);


			/**
			 * @brief Filters and sends metrics form a package to Snap
			 *
			 * @param package metrics to send
			 * @param copyMetrics function used to copy new metrics into the package
			 */
			void sendMetrics(MetricsPackage& package, std::function<void(MetricsPackage::TimedMetrics&)> copyMetrics);

		public:
			/**
			 * @brief Construct a new Snap Stream Interface object
			 */
			SnapStreamInterface();


			/**
			 * @brief Returns the plugin configuration keys and default values to Snap
			 *
			 * @return The plugin default configuration
			 */
	        const Plugin::ConfigPolicy get_config_policy() override final;

			/**
			 * @brief Returns all metrics that can be fetched by the plugin to Snap
			 *
			 * @param cfg incomming Snap configuration values
			 * @return metrics that can be fetched
			 */
     	 	std::vector<Plugin::Metric> get_metric_types(Plugin::Config cfg) override final;

			/**
			 * @brief Gets the user's requested metrics from Snap
			 *
			 * @param metsIn the user's requested metrics array
			 */
			void get_metrics_in(std::vector<Plugin::Metric> &metsIn) override final;

			/**
			 * @brief Stream metrics to Snap
			 */
			void stream_metrics() override final;


			void metricsContollerCollectedMetricsValues(const MetroCollect::MetricsController& metricsController, const MetroCollect::MetricsDiffArray& metricsDiff, const MetroCollect::MetricsDataArray& previousMetrics, const MetroCollect::MetricsDataArray& currentMetrics) override final;
			void metricsContollerCollectedMetricsStats(const MetroCollect::MetricsController& metricsController, const MetroCollect::MetricsController::MetricsStats& metricsStats) override final;
			bool metricsContollerShouldStopCollectingMetrics(const MetroCollect::MetricsController& metricsController) override final;
	};
}


/**
 * @brief MetroCollect's main function
 */
int main(int argc, char* argv[]);
