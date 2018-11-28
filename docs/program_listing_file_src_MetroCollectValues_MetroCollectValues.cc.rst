
.. _program_listing_file_src_MetroCollectValues_MetroCollectValues.cc:

Program Listing for File MetroCollectValues.cc
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollectValues_MetroCollectValues.cc>` (``src/MetroCollectValues/MetroCollectValues.cc``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // MetroCollectValues.cc
   //
   // Created on July 13th 2018
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
   #include <memory>
   #include <sstream>
   #include <thread>
   
   #if GPERFTOOLS_CPU_PROFILE
   #include <gperftools/profiler.h>
   #endif
   
   #include <MetroCollect/MetricsController.h>
   
   
   template<typename T, typename S, typename U>
   void printMetrics(const std::vector<std::pair<std::string, std::string>>& names, const MetroCollect::MetricsArray<T>& m1, const MetroCollect::MetricsArray<S>& m2, const MetroCollect::MetricsArray<U>& d) {
           std::cout << std::setw(54) << std::left << "" << std::setw(16) << "First value" << "\t" << std::setw(16) << "Second value" << "\t" << std::setw(16) << "Evolution" << "Unit" << std::endl;
           for (size_t i = 0; i < d.size(); i++)
                   std::cout << std::setw(54) << std::left << names[i].first << std::setw(16) << m1[i] << "\t" << std::setw(16) << m2[i] << "\t" << std::setw(16) << d[i] << names[i].second << std::endl;
           std::cout << std::endl << std::endl;
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
   
   
   
   int main(int argc, char* argv[]) {
           auto samplingInterval = MetroCollect::MetricsController::defaultSamplingInterval;
           size_t iterationCount = 0;
   
           if (argc > 1)
                   samplingInterval = std::chrono::milliseconds(std::atol(argv[1]));
           if (argc > 2)
                   iterationCount = std::atol(argv[2]);
   
           double factor = 1.0;
           if (samplingInterval != 0ms)
                   factor = 1000.0 / samplingInterval.count();
   
           auto sourceInterests = MetroCollect::MetricsSource::makeSourceInterests(true);
           // (*sourceInterests)[0] = true;
           auto currentMetrics = std::make_shared<MetroCollect::MetricsDataArray>(sourceInterests);
           auto previousMetrics = std::make_shared<MetroCollect::MetricsDataArray>(sourceInterests);
           MetroCollect::MetricsDiffArray diff(sourceInterests);
           auto names = getAllFieldNames(diff);
   
           currentMetrics->updateData();
   
   #if GPERFTOOLS_CPU_PROFILE
           ProfilerStart("/tmp/aa.prof");
   #endif
           do {
                   std::this_thread::sleep_for(samplingInterval);
                   std::swap(currentMetrics, previousMetrics);
   
                   currentMetrics->updateData();
                   diff.computeDiff(*currentMetrics, *previousMetrics, factor);
   
   #if PRINT_METRICS
                   printMetrics(names, *previousMetrics, *currentMetrics, diff);
                   std::cout << '\r' << std::flush;
   #endif
   
                   iterationCount--;
   #if GPERFTOOLS_CPU_PROFILE
                   ProfilerFlush();
   #endif
           } while (iterationCount != 0);
   
   #if GPERFTOOLS_CPU_PROFILE
           ProfilerStop();
   #endif
           return 0;
   }
