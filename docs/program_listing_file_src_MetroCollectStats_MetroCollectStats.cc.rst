
.. _program_listing_file_src_MetroCollectStats_MetroCollectStats.cc:

Program Listing for File MetroCollectStats.cc
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollectStats_MetroCollectStats.cc>` (``src/MetroCollectStats/MetroCollectStats.cc``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

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
   
   #include <iostream>
   #include <iomanip>
   #include <sstream>
   
   #include <MetroCollect/MetricsController.h>
   
   
   void printMetrics(const std::vector<std::pair<std::string, std::string>>& names, const MetroCollect::MetricsController::MetricsStats& s) {
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
   
   
   struct MetroCollectStats : public MetroCollect::MetricsControllerDelegate {
           size_t iterationCount;          
           std::vector<std::pair<std::string, std::string>> names;         
   
           void metricsContollerCollectedMetricsValues(const MetroCollect::MetricsController& , const MetroCollect::MetricsDiffArray& , const MetroCollect::MetricsDataArray& , const MetroCollect::MetricsDataArray& ) override {
   
           }
   
   #if PRINT_METRICS
           void metricsContollerCollectedMetricsStats(const MetroCollect::MetricsController& , const MetroCollect::MetricsController::MetricsStats& metricsStats) override {
                   printMetrics(names, metricsStats);
   #else
           void metricsContollerCollectedMetricsStats(const MetroCollect::MetricsController& , const MetroCollect::MetricsController::MetricsStats& ) override {
   #endif
           }
   
           bool metricsContollerShouldStopCollectingMetrics(const MetroCollect::MetricsController& ) override {
                   iterationCount--;
                   return iterationCount == 0;
           }
   };
   
   
   
   int main(int argc, char* argv[]) {
           auto samplingInterval = MetroCollect::MetricsController::defaultSamplingInterval;
           size_t windowLength = MetroCollect::MetricsController::defaultProcessingWindowLength;
           size_t windowOverlap = MetroCollect::MetricsController::defaultProcessingWindowOverlap;
           MetroCollectStats d;
   
           if (argc > 1)
                   samplingInterval = std::chrono::milliseconds(std::atol(argv[1]));
           if (argc > 2)
                   windowLength = std::atol(argv[2]);
           if (argc > 3)
                   windowOverlap = std::atol(argv[3]);
           if (argc > 4)
                   d.iterationCount = std::atol(argv[4]);
           else
                   d.iterationCount = 0;
   
           auto m = generateMetrics();
           d.names = getAllFieldNames(m);
           MetroCollect::MetricsController controller(d);
           controller.setSendStats(true);
           controller.setSamplingInterval(samplingInterval);
           controller.setProcessingWindow(windowLength, windowOverlap);
           controller.setRequestedMetrics(m);
   
           controller.collectMetrics();
   
           return 0;
   }
