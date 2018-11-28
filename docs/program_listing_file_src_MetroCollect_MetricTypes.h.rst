
.. _program_listing_file_src_MetroCollect_MetricTypes.h:

Program Listing for File MetricTypes.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_MetricTypes.h>` (``src/MetroCollect/MetricTypes.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // MetricTypes.h
   //
   // Created on July 23th 2018
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
   
   #include <cstdint>
   #include <vector>
   
   
   namespace MetroCollect {
           using DataValueType = int64_t;                                                                                                  
           using DiffValueType = double;                                                                                                   
   
           namespace MetricsSource {
                   namespace DataArray {
                           using Iterator = std::vector<DataValueType>::iterator;                                  
                           using ConstIterator = std::vector<DataValueType>::const_iterator;               
                   }
   
                   namespace DiffArray {
                           using Iterator = std::vector<DiffValueType>::iterator;                                  
                           using ConstIterator = std::vector<DiffValueType>::const_iterator;               
                   }
           }
   }
