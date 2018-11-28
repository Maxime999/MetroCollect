
.. _program_listing_file_src_MetroCollect_Statistics.h:

Program Listing for File Statistics.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_Statistics.h>` (``src/MetroCollect/Statistics.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

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
   
           class Statistics {
                   friend MetricsController;                                                               
                   protected:
                           enum StatsIndex : uint8_t {
                                   StatsIndexMin           = 0,                                            
                                   StatsIndexMax           = 1,                                            
                                   StatsIndexAverage       = 2,                                            
                                   StatsIndexStdDev        = 3,                                            
   
                                   // Private intermediate statistics
                                   StatsIndexIntermediateSum                       = 4,            
                                   StatsIndexIntermediateSumSquared        = 5,            
                           };
   
                   public:
                           static constexpr size_t count = 4;                                                                                                                                                              
                           static constexpr std::array<std::string_view, count> names = {"min"sv, "max"sv, "average"sv, "stdDev"sv};               
                           static constexpr std::string_view nameStatsAll = "*"sv;
   
                           enum Stats : uint64_t {
                                   StatsMin                = 1 << StatsIndexMin,                                                                           
                                   StatsMax                = 1 << StatsIndexMax,                                                                           
                                   StatsAverage    = 1 << StatsIndexAverage,                                                                       
                                   StatsStdDev             = 1 << StatsIndexStdDev,                                                                        
   
                                   StatsNone               = 0,                                                                                                            
                                   StatsAll                = StatsMin | StatsMax | StatsAverage | StatsStdDev,                     
                           };
                           static constexpr std::array<Stats, count> allStats = {StatsMin, StatsMax, StatsAverage, StatsStdDev};                   
   
                           static Stats statsFromName(const std::string_view& statName);                                   
                           static Stats statsRequiredForStat(Stats stat);                                                                  
   
                   protected:
                           static constexpr size_t extraCount = count + 2;                                                                 
   
                           enum StatsDependency : uint64_t {
                                   StatsDependencyMin                      = StatsMin,                                                                             
                                   StatsDependencyMax                      = StatsMax,                                                                             
                                   StatsDependencyStdDev           = StatsStdDev,                                                                  
                                   StatsDependencyAverage          = StatsAverage | StatsDependencyStdDev,                 
                                   StatsDependencySum                      = StatsDependencyAverage,                                               
                                   StatsDependencySumSquared       = StatsDependencyStdDev,                                                
                           };
                           static constexpr std::array<StatsDependency, extraCount> StatsDependencies = {StatsDependencyMin, StatsDependencyMax, StatsDependencyAverage, StatsDependencyStdDev, StatsDependencySum, StatsDependencySumSquared};            
           };
   
   
           template<typename T>
           constexpr inline Statistics::Stats operator| (Statistics::Stats a, T b) {
                   return static_cast<Statistics::Stats>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
           }
   
           template<typename T>
           constexpr inline Statistics::Stats& operator|= (Statistics::Stats& a, T b) {
                   a = a | b;
                   return a;
           }
   }
