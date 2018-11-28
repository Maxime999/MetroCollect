
.. _program_listing_file_src_MetroCollect_MetricsSourceArray.h:

Program Listing for File MetricsSourceArray.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_MetricsSourceArray.h>` (``src/MetroCollect/MetricsSourceArray.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // MetricsSources.h
   //
   // Created on August 8th 2018
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
   
   #include "SourceProcStat.h"
   #include "SourceProcMeminfo.h"
   #include "SourceProcNetDev.h"
   #include "SourceEthtool.h"
   
   
   namespace MetroCollect::MetricsSource {
           struct MetricsSourceArray {
                   SourceProcStat& procStat;                               
                   SourceProcMeminfo& procMemInfo;                 
                   SourceProcNetDev& procNetDev;                   
                   SourceEthtool& ethtool;                                 
   
                   static constexpr size_t count = 4;              
   
   
                   MetricsSourceArray();
   
   
                   template<typename Function>
                   void forEach(Function func) {
                           func(procStat, 0);
                           func(procMemInfo, 1);
                           func(procNetDev, 2);
                           func(ethtool, 3);
                   }
   
                   template<typename Function>
                   void forEach(Function func) const {
                           func(procStat, 0);
                           func(procMemInfo, 1);
                           func(procNetDev, 2);
                           func(ethtool, 3);
                   }
           };
   }
