
.. _program_listing_file_src_MetroCollect_SourceField.h:

Program Listing for File SourceField.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceField.h>` (``src/MetroCollect/SourceField.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // SourceField.h
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
   
   #include <string>
   #include <vector>
   
   
   namespace MetroCollect::MetricsSource {
           using FieldName = std::vector<std::string>;
   
   
           struct FieldInfo {
                   struct IndexAndDescription {
                           size_t index;
                           std::string description;
   
                           IndexAndDescription(size_t anIndex, std::string aDescription) : index(anIndex), description(std::move(aDescription)) { }
                   };
   
                   FieldName name;                                                                                                         
                   std::string description;                                                                                        
                   std::string unit;                                                                                                       
                   std::vector<IndexAndDescription> dynamicIndexes;                                        
   
   
                   FieldInfo() { }
   
                   FieldInfo(FieldName aName, std::string aDescription, std::string aUnit) :
                           name(aName), description(aDescription), unit(aUnit) { }
   
                   FieldInfo(FieldName aName, std::string aDescription, std::string aUnit, size_t anIndex, std::string aDynamicDescription) :
                           name(aName), description(aDescription), unit(aUnit), dynamicIndexes({ IndexAndDescription{anIndex, std::move(aDynamicDescription)} }) { }
   
   
                   bool isDynamic() {
                           return dynamicIndexes.size() == 0;
                   }
           };
   }
