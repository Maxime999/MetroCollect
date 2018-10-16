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
	class SnapStreamInterface final : public Plugin::StreamCollectorInterface, public MetroCollect::MetricsControllerDelegate {
		public:
			static constexpr int appVersion = 2;
			static constexpr std::string_view appName = "metrocollect"sv;

			struct MetricsPackage {
				struct TimedMetrics {
					std::chrono::system_clock::time_point timestamp;
					std::vector<MetroCollect::DiffValueType> values;
				};

				std::vector<Plugin::Metric> computedMetrics;
				std::vector<Plugin::Metric*> metricsToSend;
				std::unique_ptr<TimedMetrics> currentMetrics;
				std::unique_ptr<TimedMetrics> nextMetrics;
				std::vector<size_t> metricsTimeout;
				std::vector<size_t*> metricsTimeoutPointer;
				std::vector<bool> secondaryMetrics;

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
			static constexpr std::array appPrefix = {"cfm"sv/*, "metrocollect"sv*/};
			static constexpr std::string_view configKeySendValues = "SendValues"sv;
			static constexpr std::string_view configKeySendStats = "SendStats"sv;
			static constexpr std::string_view configKeySamplingInterval = "SamplingInterval"sv;
			static constexpr std::string_view configKeyProcessingWindowLength = "ProcessingWindowLength"sv;
			static constexpr std::string_view configKeyProcessingWindowSlidingFactor = "ProcessingWindowSlidingFactor"sv;
			static constexpr std::string_view configKeyConvertToUnitsPerSecond = "ConvertToUnitsPerSecond"sv;
			static constexpr std::string_view configKeyUnchangedMetricTimeout = "UnchangedMetricTimeout"sv;
			static constexpr std::string_view configKeyMaxCollectDuration = "MaxCollectDuration"sv;
			static constexpr std::string_view configKeyMaxMetricsBuffer = "MaxMetricsBuffer"sv;
			static constexpr std::array configKeysInt = {configKeySamplingInterval, configKeyProcessingWindowLength, configKeyProcessingWindowSlidingFactor, configKeyUnchangedMetricTimeout, configKeyMaxCollectDuration, configKeyMaxMetricsBuffer};
			static constexpr std::array configKeysBool = {configKeySendValues, configKeySendStats, configKeyConvertToUnitsPerSecond};
			static constexpr bool defaultSendValues = false;
			static constexpr bool defaultSendStats = true;
			static constexpr size_t defaultUnchangedMetricTimeout = 300;
			static constexpr std::chrono::seconds defaultMaxCollectDuration = 0s;
			static constexpr size_t defaultMaxMetricsBuffer = 0;
			static constexpr std::array<int, configKeysInt.size()> configValuesInt = {MetroCollect::MetricsController::defaultSamplingInterval.count(), MetroCollect::MetricsController::defaultProcessingWindowLength, MetroCollect::MetricsController::defaultProcessingWindowSlidingFactor, SnapStreamInterface::defaultUnchangedMetricTimeout, SnapStreamInterface::defaultMaxCollectDuration.count(), SnapStreamInterface::defaultMaxMetricsBuffer};
			static constexpr std::array<bool, configKeysBool.size()> configValuesBool = {defaultSendValues, defaultSendStats, MetroCollect::MetricsController::defaultConvertToUnitsPerSecond};
			static constexpr std::string_view statNamespaceLastComponent = "value"sv;
			static constexpr std::string_view statNamespaceDescription = "statistic"sv;

			MetroCollect::MetricsController metricsController_;

			MetroCollect::MetricsArray<MetroCollect::Statistics::Stats> requestedMetrics_;
			size_t unchangedMetricsTimeout_;

			MetricsPackage valuesPackage_;
			MetricsPackage statsPackage_;


			void setConfig(const Plugin::Config& cfg);

			void parseSnapMetrics(const std::vector<Plugin::Metric>& metrics);

			void insertAppPrefixToNamespace(std::vector<std::string>& ns);
			void createRequestedMetrics(MetricsPackage& package, std::string suffix, std::function<bool(MetroCollect::Statistics::Stats, bool*)> condition);
			void fillMetricsPackage(MetricsPackage& package);

			void sendMetrics(MetricsPackage& package, std::function<void(MetricsPackage::TimedMetrics&)> copyMetrics);

		public:
			SnapStreamInterface();

	        const Plugin::ConfigPolicy get_config_policy() override final;
     	 	std::vector<Plugin::Metric> get_metric_types(Plugin::Config cfg) override final;
			void get_metrics_in(std::vector<Plugin::Metric> &metsIn) override final;

			void stream_metrics() override final;

			void metricsContollerCollectedMetricsValues(const MetroCollect::MetricsController& metricsController, const MetroCollect::MetricsDiffArray& metricsDiff, const MetroCollect::MetricsDataArray& previousMetrics, const MetroCollect::MetricsDataArray& currentMetrics) override final;
			void metricsContollerCollectedMetricsStats(const MetroCollect::MetricsController& metricsController, const MetroCollect::MetricsController::MetricsStats& metricsStats) override final;
			bool metricsContollerShouldStopCollectingMetrics(const MetroCollect::MetricsController& metricsController) override final;
	};
}
