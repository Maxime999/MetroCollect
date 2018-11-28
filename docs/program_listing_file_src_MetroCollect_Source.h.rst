
.. _program_listing_file_src_MetroCollect_Source.h:

Program Listing for File Source.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_Source.h>` (``src/MetroCollect/Source.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // MetricsSourceArray.h
   //
   // Created on July 18th 2018
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
   
   #include <string>
   #include <string_view>
   #include <vector>
   
   #include "MetricTypes.h"
   #include "SourceField.h"
   #include "SourceInterests.h"
   #include "SourceTools.h"
   
   
   namespace MetroCollect::MetricsSource {
           class SourceBase {
                   protected:
                           SourceBase() {}                 
                           ~SourceBase() {}                
   
                   public:
                           SourceBase(const SourceBase&) = delete;                 
                           void operator=(const SourceBase&) = delete;             
   
   
                           virtual size_t fieldCount() const noexcept = 0;
   
                           virtual const std::vector<size_t> indexesOfFieldName(const FieldName& fieldName, Interests* interests = nullptr) const noexcept = 0;
   
                           virtual const std::string fieldNameSourcePrefix() const noexcept = 0;
   
                           virtual const FieldInfo fieldInfoAtIndex(size_t index) const noexcept = 0;
   
                           virtual const std::vector<FieldInfo> allFieldsInfo() const noexcept = 0;
   
   
                           virtual void fetchData(const Interests& interests, DataArray::Iterator current) = 0;
   
   
                           virtual void computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor = 1) noexcept = 0;
           };
   }
