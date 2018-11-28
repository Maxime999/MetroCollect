
.. _program_listing_file_src_MetroCollect_SourceProcMeminfo.cc:

Program Listing for File SourceProcMeminfo.cc
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceProcMeminfo.cc>` (``src/MetroCollect/SourceProcMeminfo.cc``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // SourceProcMeminfo.cc
   //
   // Created on August 23rd 2018
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
   #include <cstring>
   
   #include "SourceProcMeminfo.h"
   
   
   namespace MetroCollect::MetricsSource {
           SourceProcMeminfo::SourceProcMeminfo() {
                   resetFile(this->file_, this->buffer_, SourceProcMeminfo::filePath);
                   this->parseFields();
           }
   
   
           void SourceProcMeminfo::parseFields() {
                   bool read = readFile(this->file_, this->buffer_, SourceProcMeminfo::filePath);
                   if (!read)
                           return;
   
                   const char* buffer = this->buffer_.data();
   
                   this->fieldNames_.clear();
                   while (buffer[0] != '\0') {
                           size_t length = 0;
                           while (buffer[length] != ':')
                                   length++;
                           this->fieldNames_.emplace_back(buffer, length);
                           buffer += length;
                           while (buffer[0] != '\n')
                                   buffer++;
                           buffer++;
                   }
   
                   for (auto& fieldName : this->fieldNames_) {
                           bool wasCapitalized = false;
                           for (size_t i = 0; i < fieldName.size(); i++) {
                                   if (!isalnum(fieldName[i]) && fieldName[i] != '_')
                                           fieldName[i] = '_';
                                   else if (isupper(fieldName[i])) {
                                           if (i > 0 && fieldName[i - 1] != '_' && islower(fieldName[i - 1]) && !wasCapitalized) {
                                                   fieldName.insert(i, "_");
                                                   i++;
                                           }
                                           fieldName[i] = tolower(fieldName[i]);
                                           wasCapitalized = true;
                                   } else
                                           wasCapitalized = false;
                           }
                           while (fieldName.back() == '_')
                                   fieldName.erase(fieldName.size() - 1);
                           while (fieldName.front() == '_')
                                   fieldName.erase(0);
                   }
           }
   
   
           size_t SourceProcMeminfo::fieldCount() const noexcept {
                   return this->fieldNames_.size();
           }
   
           const std::vector<size_t> SourceProcMeminfo::indexesOfFieldName(const FieldName& fieldName, Interests* interests) const noexcept {
                   if (fieldName.front() != SourceProcMeminfo::sourcePrefix)
                           return {};
   
                   auto itr = std::find(this->fieldNames_.begin(), this->fieldNames_.end(), fieldName[1]);
                   if (itr != this->fieldNames_.end()) {
                           if (interests)
                                   interests->set(0);
                           return {static_cast<size_t>(std::distance(this->fieldNames_.begin(), itr))};
                   }
                   return {};
           }
   
           const std::string SourceProcMeminfo::fieldNameSourcePrefix() const noexcept {
                   return std::string(SourceProcMeminfo::sourcePrefix);
           }
   
           const FieldInfo SourceProcMeminfo::fieldInfoAtIndex(size_t index) const noexcept {
                   FieldName name = {std::string(SourceProcMeminfo::sourcePrefix), this->fieldNames_[index]};
                   std::string unit = findUnit(this->fieldNames_[index], SourceProcMeminfo::fieldUnitsAssociation, SourceProcMeminfo::defaultUnit);
                   return {name, "Memory metric: " + this->fieldNames_[index], unit};
           }
   
           const std::vector<FieldInfo> SourceProcMeminfo::allFieldsInfo() const noexcept {
                   std::vector<FieldInfo> info;
                   for (size_t i = 0; i < this->fieldNames_.size(); i++)
                           info.push_back(this->fieldInfoAtIndex(i));
                   return info;
           }
   
   
           void SourceProcMeminfo::fetchData(const Interests& interests, DataArray::Iterator current) {
                   bool read = false;
                   if (interests.none() || !(read = readFile(this->file_, this->buffer_, SourceProcMeminfo::filePath))) {
                           std::fill_n(current, this->fieldCount(), 0);
                           return;
                   }
   
                   const char* buffer = this->buffer_.data();
                   while (buffer[0] != '\0') {
                           while (buffer[0] != ':')
                                   buffer++;
                           buffer++;
                           *current = static_cast<DataValueType>(parseUint(buffer));
                           while (buffer[0] == ' ')
                                   buffer++;
                           switch (buffer[0]) {
                                   case 'T':
                                   case 't':
                                           *current *= 1024;
                                           [[fallthrough]];
                                   case 'G':
                                   case 'g':
                                           *current *= 1024;
                                           [[fallthrough]];
                                   case 'M':
                                   case 'm':
                                           *current *= 1024;
                                           [[fallthrough]];
                                   case 'K':
                                   case 'k':
                                           *current *= 1024;
                                           buffer += 2;
                                           break;
                                   default:
                                           break;
                           }
                           current++;
                           while (buffer[0] != '\n')
                                   buffer++;
                           buffer++;
                   }
           }
   
   
           void SourceProcMeminfo::computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator, double) noexcept {
                   if (interests.none())
                           return;
   
                   std::copy_n(current, this->fieldNames_.size(), diff);
           }
   }
