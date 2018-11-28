
.. _program_listing_file_src_MetroCollect_SourceTools.h:

Program Listing for File SourceTools.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceTools.h>` (``src/MetroCollect/SourceTools.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // SourceTools.h
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
   #include <cctype>
   #include <fstream>
   #include <string>
   #include <string_view>
   #include <vector>
   
   using namespace std::literals;
   
   
   namespace MetroCollect::MetricsSource {
           inline uint64_t parseUint(const char*& buffer) noexcept {
                   uint64_t result = 0;
                   while (!std::isdigit(*buffer))
                           buffer++;
                   while (std::isdigit(*buffer)) {
                           result = (result << 1) + (result << 3) + *buffer - '0';
                           buffer++;
                   }
                   return result;
           }
   
           bool resetFile(std::ifstream& file, std::vector<char>& buffer, const std::string_view& path);
   
           bool readFile(std::ifstream& file, std::vector<char>& buffer, const std::string_view& path);
   
   
           struct KeyUnit {
                   std::string_view key;           //<! Key to find in field name for the unit to be applicable
                   std::string_view unit;          //<! The unit
   
                   constexpr KeyUnit() : key(""sv), unit(""sv) { }                                                                                                         //<! Default constructor
                   constexpr KeyUnit(std::string_view aKey, std::string_view aUnit) : key(aKey), unit(aUnit) { }           //<! Constructor with values
           };
   
           template<size_t N>
           std::string findUnit(const std::string& name, const std::array<KeyUnit, N>& keyUnitAssociation, const std::string_view& defaultUnit) {
                   for (const auto& val : keyUnitAssociation) {
                           if (name.find(val.key) != std::string::npos)
                                   return std::string(val.unit);
                   }
                   return std::string(defaultUnit);
           }
   
   
           int alphanumCompare(const char* l, const char* r);
   }
