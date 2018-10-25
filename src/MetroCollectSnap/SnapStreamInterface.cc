//
// SnapStreamInterface.cc
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

#include <algorithm>

#include "SnapStreamInterface.h"

using namespace std::literals;


namespace SnapInterface {
	SnapStreamInterface::SnapStreamInterface() :
		metricsController_(*this)
	{ }


	void SnapStreamInterface::setConfig(const Plugin::Config& cfg) {
		bool sendValues = SnapStreamInterface::defaultSendValues;
		bool sendStats = SnapStreamInterface::defaultSendStats;
		std::chrono::milliseconds sampling = MetroCollect::MetricsController::defaultSamplingInterval;
		size_t windowLength = MetroCollect::MetricsController::defaultProcessingWindowLength;
		size_t windowOverlap = MetroCollect::MetricsController::defaultProcessingWindowOverlap;
		size_t unchangedTimeout = SnapStreamInterface::defaultUnchangedMetricTimeout;
		bool convert = MetroCollect::MetricsController::defaultConvertToUnitsPerSecond;

		if (cfg.has_bool_key(std::string(SnapStreamInterface::configKeySendValues)))
			sendValues = cfg.get_bool(std::string(SnapStreamInterface::configKeySendValues));
		if (cfg.has_bool_key(std::string(SnapStreamInterface::configKeySendStats)))
			sendStats = cfg.get_bool(std::string(SnapStreamInterface::configKeySendStats));

		if (cfg.has_int_key(std::string(SnapStreamInterface::configKeySamplingInterval)))
			sampling = std::chrono::milliseconds(cfg.get_int(std::string(SnapStreamInterface::configKeySamplingInterval)));
		if (cfg.has_int_key(std::string(SnapStreamInterface::configKeyProcessingWindowLength)))
			windowLength = cfg.get_int(std::string(SnapStreamInterface::configKeyProcessingWindowLength));
		if (cfg.has_int_key(std::string(SnapStreamInterface::configKeyProcessingWindowOverlap)))
			windowOverlap = cfg.get_int(std::string(SnapStreamInterface::configKeyProcessingWindowOverlap));

		if (cfg.has_bool_key(std::string(SnapStreamInterface::configKeyConvertToUnitsPerSecond)))
			convert = cfg.get_bool(std::string(SnapStreamInterface::configKeyConvertToUnitsPerSecond));
		if (cfg.has_int_key(std::string(SnapStreamInterface::configKeyUnchangedMetricTimeout)))
			unchangedTimeout = cfg.get_int(std::string(SnapStreamInterface::configKeyUnchangedMetricTimeout));

		this->metricsController_.setSendValues(sendValues);
		this->metricsController_.setSendStats(sendStats);
		this->metricsController_.setSamplingInterval(sampling);
		this->metricsController_.setProcessingWindow(windowLength, windowOverlap);
		this->metricsController_.setConvertToUnitsPerSeconds(convert);
		this->unchangedMetricsTimeout_ = unchangedTimeout;
	}


	void SnapStreamInterface::parseSnapMetrics(const std::vector<Plugin::Metric>& metrics) {
		for (auto& value : *this->requestedMetrics_.sourceInterests())
			value = false;
		for (auto& value : this->requestedMetrics_)
			value = MetroCollect::Statistics::StatsNone;

		MetroCollect::MetricsSource::FieldName fieldName;

		for (const auto& metric : metrics) {
			fieldName.clear();
			for (size_t i = SnapStreamInterface::appPrefix.size(); i < metric.ns().size() - 2; i++)
				fieldName.push_back(metric.ns()[i].get_value());
			auto fieldIndexes = this->requestedMetrics_.indexesOfFieldName(fieldName);
			auto stat = MetroCollect::Statistics::statsFromName(metric.ns()[metric.ns().size() - 2].get_value());
			for (size_t i : fieldIndexes)
				this->requestedMetrics_[i] |= MetroCollect::Statistics::statsRequiredForStat(stat);
		}

		this->metricsController_.setRequestedMetrics(this->requestedMetrics_);
	}


	void SnapStreamInterface::insertAppPrefixToNamespace(std::vector<std::string>& ns) {
		std::transform(SnapStreamInterface::appPrefix.begin(), SnapStreamInterface::appPrefix.end(), std::inserter(ns, ns.begin()), [&](auto s) { return std::string(s); });
	}

	void SnapStreamInterface::createRequestedMetrics(MetricsPackage& package, std::string suffix, std::function<bool(MetroCollect::Statistics::Stats, bool*)> condition) {
		for (size_t i = 0; i < this->requestedMetrics_.size(); i++) {
			bool isSecondaryMetric = false;
			if (condition(this->requestedMetrics_[i], &isSecondaryMetric)) {
				auto fieldInfo = this->requestedMetrics_.fieldInfoAtIndex(i);
				this->insertAppPrefixToNamespace(fieldInfo.name);
				if (!suffix.empty())
					fieldInfo.name.push_back(suffix);
				Plugin::Namespace ns(fieldInfo.name);
				for (const auto& dynamicIndex: fieldInfo.dynamicIndexes)
					ns[dynamicIndex.index + SnapStreamInterface::appPrefix.size()].set_name(dynamicIndex.description);
				if (!suffix.empty())
					ns[ns.size() - 1].set_name(std::string(SnapStreamInterface::statNamespaceDescription));

				package.computedMetrics.emplace_back(ns, "", "");
				auto itr = std::lower_bound(this->metricsController_.requestedMetrics().begin(), this->metricsController_.requestedMetrics().end(), i);
				auto index = std::distance(this->metricsController_.requestedMetrics().begin(), itr);
				package.metricsTimeoutPointer.push_back(&package.metricsTimeout[index]);
				package.secondaryMetrics.push_back(isSecondaryMetric);
			}
		}
	}

	void SnapStreamInterface::fillMetricsPackage(MetricsPackage& package) {
		package.metricsToSend.reserve(package.computedMetrics.size());
		if (this->unchangedMetricsTimeout_ < ((size_t)-1)) {
			for (size_t i = 0; i < package.metricsTimeout.size(); i++)
				package.metricsTimeout[i] = (i * this->unchangedMetricsTimeout_) / package.metricsTimeout.size() + 1;
		}
	}


	void SnapStreamInterface::sendMetrics(MetricsPackage& package, std::function<void(MetricsPackage::TimedMetrics&)> copyMetrics) {
		bool shouldSend = true;
		if (!package.currentMetrics) {
			shouldSend = ((bool)package.nextMetrics);
			package.currentMetrics = std::make_unique<MetricsPackage::TimedMetrics>();
			package.currentMetrics->values.resize(package.computedMetrics.size());
		}
		std::swap(package.currentMetrics, package.nextMetrics);

		copyMetrics(*package.nextMetrics);

		if (!shouldSend)
			return;

		for (size_t i = 0; i < package.computedMetrics.size(); i++) {
			if (package.currentMetrics->values[i] != package.nextMetrics->values[i])
				*package.metricsTimeoutPointer[i] = 1;
			else if (*package.metricsTimeoutPointer[i] > 3 && package.currentMetrics->values[i] != 0)
				*package.metricsTimeoutPointer[i] = 2;
		}

		package.metricsToSend.clear();
		for (size_t i = 0; i < package.computedMetrics.size(); i++) {
			if (*package.metricsTimeoutPointer[i] <= 1 || (*package.metricsTimeoutPointer[i] <= 3 && !package.secondaryMetrics[i])) {
				package.computedMetrics[i].set_data(package.currentMetrics->values[i]);
				package.computedMetrics[i].set_timestamp(package.currentMetrics->timestamp);
				package.metricsToSend.push_back(&package.computedMetrics[i]);
			}
		}
		this->send_metrics(package.metricsToSend);

		if (this->unchangedMetricsTimeout_ < ((size_t)-1)) {
			for (size_t i = 0; i < package.metricsTimeout.size(); i++) {
				if (package.metricsTimeout[i] == 0)
					package.metricsTimeout[i] = (i * this->unchangedMetricsTimeout_) / package.metricsTimeout.size() + 3;
				else if(package.metricsTimeout[i] == 2 || package.metricsTimeout[i] == 3)
					package.metricsTimeout[i] += this->unchangedMetricsTimeout_ - 1;
				else
					package.metricsTimeout[i]--;
			}
		} else
			std::fill(package.metricsTimeout.begin(), package.metricsTimeout.end(), ((size_t)-1));
	}


	const Plugin::ConfigPolicy SnapStreamInterface::get_config_policy()  {
		Plugin::ConfigPolicy policy;
		std::vector<std::string> ns;
		std::vector<std::string> baseNamespace;
		this->insertAppPrefixToNamespace(baseNamespace);
		for (size_t i = 0; i < SnapStreamInterface::configKeysInt.size(); i++) {
			ns = baseNamespace;
			ns.emplace_back(SnapStreamInterface::configKeysInt[i]);
			policy.add_rule(ns, Plugin::IntRule{std::string(SnapStreamInterface::configKeysInt[i]), {SnapStreamInterface::configValuesInt[i], false}});
		}
		for (size_t i = 0; i < SnapStreamInterface::configKeysBool.size(); i++) {
			ns = baseNamespace;
			ns.emplace_back(SnapStreamInterface::configKeysBool[i]);
			policy.add_rule(ns, Plugin::BoolRule{std::string(SnapStreamInterface::configKeysBool[i]), {SnapStreamInterface::configValuesBool[i], false}});
		}
		return policy;
	}

	std::vector<Plugin::Metric> SnapStreamInterface::get_metric_types(Plugin::Config cfg)  {
		std::vector<Plugin::Metric> metrics;
		this->setConfig(cfg);

		auto fieldsInfo = this->requestedMetrics_.allFieldsInfo();
		for (auto& fieldInfo : fieldsInfo) {
			this->insertAppPrefixToNamespace(fieldInfo.name);
			Plugin::Namespace ns{fieldInfo.name};
			for (const auto& dynamicIndex: fieldInfo.dynamicIndexes)
				ns[dynamicIndex.index + SnapStreamInterface::appPrefix.size()].set_name(dynamicIndex.description);
			ns.add_dynamic_element(std::string(SnapStreamInterface::statNamespaceDescription));
			ns.add_static_element(std::string(SnapStreamInterface::statNamespaceLastComponent));
			metrics.emplace_back(ns, fieldInfo.unit, fieldInfo.description);
		}

		return metrics;
	}

	void SnapStreamInterface::stream_metrics()  {
		if (this->valuesPackage_.computedMetrics.size() == 0 && this->statsPackage_.computedMetrics.size() == 0)
			return;

		this->metricsController_.collectMetrics();
	}

	void SnapStreamInterface::get_metrics_in(std::vector<Plugin::Metric> &metsIn)  {
		if (metsIn.size() == 0)
			return;

		this->setConfig(metsIn.front().get_config());

		this->parseSnapMetrics(metsIn);

		if (this->metricsController_.sendValues()) {
			this->valuesPackage_.clear(this->metricsController_.requestedMetrics().size());
			this->createRequestedMetrics(this->valuesPackage_, std::string(SnapStreamInterface::statNamespaceLastComponent), [&](MetroCollect::Statistics::Stats value, bool* isSecondarayMetric) {
				*isSecondarayMetric = false;
				return value != 0;
			});
			this->fillMetricsPackage(this->valuesPackage_);
		}

		if (this->metricsController_.sendStats()) {
			this->statsPackage_.clear(this->metricsController_.requestedMetrics().size());
			for (size_t statIndex = 0; statIndex < MetroCollect::Statistics::count; statIndex++) {
				this->createRequestedMetrics(this->statsPackage_, std::string(MetroCollect::Statistics::names[statIndex]), [&](MetroCollect::Statistics::Stats value, bool* isSecondarayMetric) {
					*isSecondarayMetric = (MetroCollect::Statistics::allStats[statIndex] != MetroCollect::Statistics::StatsAverage);
					return value & MetroCollect::Statistics::allStats[statIndex];
				});
			}
			this->fillMetricsPackage(this->statsPackage_);
		}
	}


	void SnapStreamInterface::metricsContollerCollectedMetricsValues(const MetroCollect::MetricsController& , const MetroCollect::MetricsDiffArray& metricsDiff, const MetroCollect::MetricsDataArray& , const MetroCollect::MetricsDataArray& ) {
		this->sendMetrics(this->valuesPackage_, [&](MetricsPackage::TimedMetrics& array) {
			size_t index = 0;
			array.timestamp = metricsDiff.endTime();
			for (size_t metricIndex : this->metricsController_.requestedMetrics()) {
				array.values[index] = metricsDiff[metricIndex];
				index++;
			}
		});
	}

	void SnapStreamInterface::metricsContollerCollectedMetricsStats(const MetroCollect::MetricsController& , const MetroCollect::MetricsController::MetricsStats& metricsStats) {
		this->sendMetrics(this->statsPackage_, [&](MetricsPackage::TimedMetrics& array) {
			size_t index = 0;
			array.timestamp = metricsStats.min.endTime();
			metricsStats.forEach([&](const auto& statsMetrics, MetroCollect::Statistics::Stats ) {
				for (const auto& val : statsMetrics.indexedValues()) {
					array.values[index] = val.value;
					index++;
				}
			});
		});
	}

	bool SnapStreamInterface::metricsContollerShouldStopCollectingMetrics(const MetroCollect::MetricsController& ) {
		return this->context_cancelled();
	}
}


int main(int argc, char* argv[]) {
    Plugin::Meta meta(Plugin::Type::StreamCollector, std::string(SnapInterface::SnapStreamInterface::appName), SnapInterface::SnapStreamInterface::appVersion, Plugin::RpcType::GRPCStream);
	meta.concurrency_count = 1;
	meta.exclusive = false;
	meta.strategy = Plugin::Strategy::Sticky;
	meta.cache_ttl = 1ms;
    SnapInterface::SnapStreamInterface plugin;
	Plugin::start_stream_collector(argc, argv, &plugin, meta);
	return 0;
}
