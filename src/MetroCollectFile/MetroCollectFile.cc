//
// MetroCollectStats.cc
//
// Created on July 26th 2018
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

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <MetroCollect/MetricsController.h>


/**
 * @brief Print metrics on screen
 *
 * @tparam T Type of first MetricsArray
 * @tparam S Type of second MetricsArray
 * @tparam U Type of third MetricsArray
 * @param names array of names of the metrics
 * @param m1 First MetricsDataArray: earlier values
 * @param m2 Second MetricsDataArray: latest values
 * @param d Third MetricsDiffArray: differences
 */
template<typename T, typename S, typename U>
void printMetricsValues(const std::vector<std::pair<std::string, std::string>>& names, const MetroCollect::MetricsArray<T>& m1, const MetroCollect::MetricsArray<S>& m2, const MetroCollect::MetricsArray<U>& d) {
	std::cout << std::setw(54) << std::left << "" << std::setw(16) << "First value" << "\t" << std::setw(16) << "Second value" << "\t" << std::setw(16) << "Evolution" << "Unit" << std::endl;
	for (size_t i = 0; i < d.size(); i++)
		std::cout << std::setw(54) << std::left << names[i].first << std::setw(16) << m1[i] << "\t" << std::setw(16) << m2[i] << "\t" << std::setw(16) << d[i] << names[i].second << std::endl;
	std::cout << std::endl << std::endl;
}


/**
 * @brief Print metric stats on screen
 *
 * @param names array of names of the metrics
 * @param s Metrics Stats Arrays to print
 */
void printMetricsStats(const std::vector<std::pair<std::string, std::string>>& names, const MetroCollect::MetricsController::MetricsStats& s) {
	std::cout << std::setw(54) << " ";
	for (const auto& st : MetroCollect::Statistics::names)
		std::cout << std::setw(16) << std::left << st << "\t";
	std::cout << "Unit" << std::endl;
	for (size_t i = 0; i < s.min.size(); i++) {
		std::cout << std::setw(54) << std::left << names[i].first;
		s.forEach([&](const auto& st, MetroCollect::Statistics::Stats ) {
			std::cout << std::setw(16) << st[i] << "\t";
		});
		std::cout << names[i].second << std::endl;
	}
	std::cout << std::endl << std::endl;
}


/**
 * @brief Generate metric request to give to the MetricsController
 *
 * @return auto requested metrics list
 */
auto generateMetrics() {
	auto interests = MetroCollect::MetricsSource::makeSourceInterests(true);
	MetroCollect::MetricsArray<MetroCollect::Statistics::Stats> request{interests};
	// (*interests)[0] = true;
	// auto [begin, end] = request.metricsSliceForSource(0);
	// for (auto itr = begin; itr < end; itr++)
	for (auto itr = request.begin(); itr < request.end(); itr++)
		*itr = MetroCollect::Statistics::StatsAll;
	return request;
}


/**
 * @brief Get fields name of all metrics
 *
 * @tparam T Type of MetricsArray
 * @param metricsArray MetricArray to get metric names from
 * @return Array of metric names
 */
template<typename T>
auto getAllFieldNames(const MetroCollect::MetricsArray<T>& metricsArray) {
	std::vector<MetroCollect::MetricsSource::FieldInfo> fields(metricsArray.fieldCount());
	for (size_t i = 0; i < fields.size(); i++)
		fields[i] = metricsArray.fieldInfoAtIndex(i);

	std::vector<std::pair<std::string, std::string>> names;
	names.reserve(fields.size());
	std::stringstream ss;
	for (const auto& field : fields) {
		ss.str(std::string());
		for (const auto& part : field.name)
			ss << "/" << part;
		names.push_back(std::pair{ss.str(), field.unit});
	}
	return names;
}


/**
 * @brief Small struct to implement MetricsController delegate
 */
struct MetroCollectFile : public MetroCollect::MetricsControllerDelegate {
	size_t i = 0;													//!< Iteration index
	size_t iterationCount;											//!< Number of iterations remaining to be done
	std::vector<std::pair<std::chrono::duration<double, std::milli>, std::vector<MetroCollect::DiffValueType>>> values;		//!< Array of all collected metrics and their relative timestamps
	std::vector<std::pair<std::string, std::string>> names;			//!< Name of collected metrics
	std::chrono::time_point<std::chrono::system_clock> start;		//!< Start time of metric collection

	/**
	 * @brief Update members
	 *
	 * @param iterations sets the number of iterations
	 * @param metricsNames sets the metric names
	 */
	void set(size_t iterations, std::vector<std::pair<std::string, std::string>> metricsNames) {
		this->iterationCount = iterations;
		this->names = metricsNames;
		this->values.clear();
		this->values.resize(iterations, {std::chrono::duration<double, std::milli>::zero(), std::vector<MetroCollect::DiffValueType>(metricsNames.size())});
		this->start = std::chrono::system_clock::now();
	}

	void metricsContollerCollectedMetricsValues(const MetroCollect::MetricsController& , const MetroCollect::MetricsDiffArray& diff, const MetroCollect::MetricsDataArray& , const MetroCollect::MetricsDataArray& ) override {
		this->values[i].first = diff.endTime() - this->start;
		std::copy(diff.begin(), diff.end(), this->values[i].second.begin());
		i++;
	}

	void metricsContollerCollectedMetricsStats(const MetroCollect::MetricsController& , const MetroCollect::MetricsController::MetricsStats& ) override {
	}

	bool metricsContollerShouldStopCollectingMetrics(const MetroCollect::MetricsController& ) override {
		iterationCount--;
		return iterationCount == 0;
	}
};



/**
 * @brief MetroCollectFile main function
 *
 * @param argc Argument count (expected: 3)
 * @param argv Arguments values (sampling interval; iteration count; file path to write to)
 * @return int Return code
 */
int main(int argc, char* argv[]) {
	auto samplingInterval = MetroCollect::MetricsController::defaultSamplingInterval;
	size_t iterations = 10;
	const char* file = "output.csv";
	MetroCollectFile d;

	if (argc > 1)
		samplingInterval = std::chrono::milliseconds(std::atol(argv[1]));
	if (argc > 2)
		iterations = std::atol(argv[2]);
	if (argc > 3)
		file = argv[3];

	auto m = generateMetrics();
	d.set(iterations, getAllFieldNames(m));
	MetroCollect::MetricsController controller(d);
	controller.setSendValues(true);
	controller.setSamplingInterval(samplingInterval);
	controller.setProcessingWindow(1, 1);
	controller.setConvertToUnitsPerSeconds(false);
	controller.setRequestedMetrics(m);

	controller.collectMetrics();

	std::ofstream output;
	output.open(file);

	output << "name,unit";
	for (size_t j = 0; j < iterations; j++)
		output << "," << d.values[j].first.count();
	output << "\n";
	for (size_t i = 0; i < d.names.size(); i++) {
		output << d.names[i].first << "," << d.names[i].second;
		for (size_t j = 0; j < iterations; j++)
			output << "," << d.values[j].second[i];
		output << "\n";
	}

	output.close();
	return 0;
}
