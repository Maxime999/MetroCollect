
.. _program_listing_file_src_MetroCollect_SourceProcStat.cc:

Program Listing for File SourceProcStat.cc
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceProcStat.cc>` (``src/MetroCollect/SourceProcStat.cc``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // SourceProcStat.cc
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
   
   #include <algorithm>
   #include <cstring>
   #include <numeric>
   
   #include "SourceProcStat.h"
   
   
   namespace MetroCollect::MetricsSource {
           SourceProcStat::SourceProcStat() {
                   resetFile(this->file_, this->buffer_, SourceProcStat::filePath);
                   this->parseFields();
           }
   
   
           void SourceProcStat::parseFields() {
                   bool read = readFile(this->file_, this->buffer_, SourceProcStat::filePath);
                   if (!read)
                           return;
   
                   const char* buffer = this->buffer_.data();
   
                   this->cpuCount_ = 0;
                   while (std::strncmp(buffer, SourceProcStat::cpuField.data(), SourceProcStat::cpuField.size()) == 0) {
                           this->cpuCount_++;
                           buffer = std::strchr(buffer, '\n') + 1;
                   }
                   this->cpuCount_--;
   
                   this->otherFieldNames_.clear();
                   while (buffer[0] != '\0') {
                           size_t length = 0;
                           while (buffer[length] != ' ')
                                   length++;
                           this->otherFieldNames_.emplace_back(buffer, length);
                           buffer += length;
                           while (buffer[0] != '\n')
                                   buffer++;
                           buffer++;
                   }
           }
   
   
           size_t SourceProcStat::fieldCount() const noexcept {
                   return this->otherFieldNames_.size() + SourceProcStat::cpuFieldCount + this->cpuCount_ * SourceProcStat::cpuFieldCount;
           }
   
           const std::vector<size_t> SourceProcStat::indexesOfFieldName(const FieldName& fieldName, Interests* interests) const noexcept {
                   if (fieldName.front() != SourceProcStat::sourcePrefix)
                           return {};
   
                   if (fieldName[1] == SourceProcStat::fieldNameOtherPrefix) {
                           auto itr = std::find(this->otherFieldNames_.begin(), this->otherFieldNames_.end(), fieldName[2]);
                           if (itr != this->otherFieldNames_.end()) {
                                   if (interests)
                                           interests->set(0);
                                   return {static_cast<size_t>(std::distance(this->otherFieldNames_.begin(), itr))};
                           } else
                                   return {};
                   }
                   else if (fieldName[1] == SourceProcStat::fieldNameAllCoresPrefix) {
                           auto itr = std::find(SourceProcStat::cpuFieldNames.begin(), SourceProcStat::cpuFieldNames.end(), fieldName[2]);
                           if (itr != SourceProcStat::cpuFieldNames.end()) {
                                   if (interests)
                                           interests->set(1);
                                   return {this->otherFieldNames_.size() + std::distance(SourceProcStat::cpuFieldNames.begin(), itr)};
                           } else
                                   return {};
                   }
                   else if (fieldName[1] == SourceProcStat::fieldNameCorePrefix) {
                           auto itr = std::find(SourceProcStat::cpuFieldNames.begin(), SourceProcStat::cpuFieldNames.end(), fieldName[3]);
                           if (itr == SourceProcStat::cpuFieldNames.end())
                                   return {};
                           size_t baseIndex = this->otherFieldNames_.size() + SourceProcStat::cpuFieldCount + std::distance(SourceProcStat::cpuFieldNames.begin(), itr) ;
                           if (fieldName[2] == SourceProcStat::fieldNameCoreAll) {
                                   std::vector<size_t> indexes;
                                   for (size_t cpuIndex = 0; cpuIndex < this->cpuCount_; cpuIndex++)
                                           indexes.push_back(baseIndex + SourceProcStat::cpuFieldCount * cpuIndex);
                                   if (interests)
                                           interests->set(1);
                                   return indexes;
                           }
                           else {
                                   size_t cpuIndex = 0;
                                   try {
                                           cpuIndex = std::stoll(fieldName[2]);
                                   } catch (...) {
                                           return {};
                                   }
                                   if (cpuIndex < this->cpuCount_) {
                                           if (interests)
                                                   interests->set(1);
                                           return {baseIndex + SourceProcStat::cpuFieldCount * cpuIndex};
                                   }
                                   return {};
                           }
                   }
                   return {};
           }
   
           const std::string SourceProcStat::fieldNameSourcePrefix() const noexcept {
                   return std::string(SourceProcStat::sourcePrefix);
           }
   
           const FieldInfo SourceProcStat::fieldInfoAtIndex(size_t index) const noexcept {
                   if (index < this->otherFieldNames_.size()) {
                           FieldName name = {std::string(SourceProcStat::sourcePrefix), std::string(SourceProcStat::fieldNameOtherPrefix), this->otherFieldNames_[index]};
                           return {name, "CPU metric augmentation: " + this->otherFieldNames_[index], std::string(SourceProcStat::defaultUnit)};
                   } else if (index < this->otherFieldNames_.size() + SourceProcStat::cpuFieldCount) {
                           FieldName name = {std::string(SourceProcStat::sourcePrefix), std::string(SourceProcStat::fieldNameAllCoresPrefix), std::string(SourceProcStat::cpuFieldNames[index - this->otherFieldNames_.size()])};
                           return {name, "CPU aggregated metric: " + std::string(SourceProcStat::cpuFieldNames[index - this->otherFieldNames_.size()]), std::string(SourceProcStat::percentUnit)};
                   } else {
                           size_t cpuIndex = (index - this->otherFieldNames_.size()) / SourceProcStat::cpuFieldCount - 1;
                           size_t fieldIndex = (index - this->otherFieldNames_.size()) % SourceProcStat::cpuFieldCount;
                           FieldName name = {std::string(SourceProcStat::sourcePrefix), std::string(SourceProcStat::fieldNameCorePrefix), std::to_string(cpuIndex), std::string(SourceProcStat::cpuFieldNames[fieldIndex])};
                           return {name, "CPU core metric: " + std::string(SourceProcStat::cpuFieldNames[fieldIndex]), std::string(SourceProcStat::percentUnit), 2, std::string(SourceProcStat::fieldNameCoreDescription)};
                   }
           }
   
           const std::vector<FieldInfo> SourceProcStat::allFieldsInfo() const noexcept {
                   std::vector<FieldInfo> info;
                   for (size_t i = 0; i < this->otherFieldNames_.size() + SourceProcStat::cpuFieldCount; i++)
                           info.push_back(this->fieldInfoAtIndex(i));
                   for (size_t i = 0; i < SourceProcStat::cpuFieldCount; i++) {
                           info.push_back(this->fieldInfoAtIndex(this->otherFieldNames_.size() + SourceProcStat::cpuFieldCount + i));
                           info.back().name[info.back().dynamicIndexes[0].index] = SourceProcStat::fieldNameCoreAll;
                   }
                   return info;
           }
   
   
           void SourceProcStat::fetchData(const Interests& interests, DataArray::Iterator current) {
                   bool read = false;
                   if (interests.none() || !(read = readFile(this->file_, this->buffer_, SourceProcStat::filePath))) {
                           std::fill_n(current, this->fieldCount(), 0);
                           return;
                   }
   
                   const char* buffer = this->buffer_.data();
   
                   if (interests.isSet(1)) {
                           size_t index = 0;
                           size_t remainingCPUs = this->cpuCount_ + 1;
                           while (remainingCPUs > 0 && std::strncmp(buffer, SourceProcStat::cpuField.data(), SourceProcStat::cpuField.size()) == 0) {
                                   buffer += SourceProcStat::cpuField.size();
                                   while (buffer[0] != ' ')
                                           buffer++;
                                   while(buffer[0] != '\n') {
                                           buffer++;
                                           current[index + this->otherFieldNames_.size()] = static_cast<DataValueType>(parseUint(buffer));
                                           index++;
                                   }
                                   buffer++;
                                   while (index % SourceProcStat::cpuFieldCount != 0) {
                                           current[index + this->otherFieldNames_.size()] = static_cast<DataValueType>(0);
                                           index++;
                                   }
                                   remainingCPUs--;
                           }
                   }
   
                   if (interests.isSet(0)) {
                           while (std::strncmp(buffer, SourceProcStat::cpuField.data(), SourceProcStat::cpuField.size()) == 0) {
                                   while (buffer[0] != '\n')
                                           buffer++;
                                   buffer++;
                           }
   
                           size_t remainingFields = this->otherFieldNames_.size();
                           while (remainingFields > 0 && buffer[0] != '\0') {
                                   while (buffer[0] != ' ')
                                           buffer++;
                                   *current = static_cast<DataValueType>(parseUint(buffer));
                                   current++;
                                   while (buffer[0] != '\n')
                                           buffer++;
                                   buffer++;
                                   remainingFields--;
                           }
                   }
           }
   
           void SourceProcStat::computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor) noexcept {
                   if (interests.isSet(0)) {
                           for (size_t i = 0; i < this->otherFieldNames_.size(); i++) {
                                   if (i == 4 || i == 5) // procs_running and procs_blocked
                                           *diff = static_cast<DiffValueType>(*current);
                                   else
                                           *diff = static_cast<DiffValueType>((*current - *previous) * factor);
                                   diff++;
                                   current++;
                                   previous++;
                           }
                   }
   
                   if (interests.isSet(1)) {
                           for (size_t cpuIndex = 0; cpuIndex < this->cpuCount_ + 1; cpuIndex++) {
                                   auto currentSum = std::accumulate(current, current + SourceProcStat::cpuFieldCount, static_cast<DiffValueType>(0));
                                   auto previousSum = std::accumulate(previous, previous + SourceProcStat::cpuFieldCount, static_cast<DiffValueType>(0));
   
                                   if (currentSum == previousSum) {
                                           for (size_t i = 0; i < SourceProcStat::cpuFieldCount; i++)
                                                   diff[i] = static_cast<DiffValueType>(0);
                                           diff[3] = static_cast<DiffValueType>(100);
                                           diff += SourceProcStat::cpuFieldCount;
                                           current += SourceProcStat::cpuFieldCount;
                                           previous += SourceProcStat::cpuFieldCount;
                                   } else {
                                           double invSumDiff = 100.0 / (currentSum - previousSum);
                                           if (cpuIndex == 0)
                                                   invSumDiff *= this->cpuCount_;
                                           for (size_t i = 0; i < SourceProcStat::cpuFieldCount; i++) {
                                                   *diff = static_cast<DiffValueType>((*current - *previous) * invSumDiff);
                                                   diff++;
                                                   current++;
                                                   previous++;
                                           }
                                   }
                           }
                   }
           }
   }
