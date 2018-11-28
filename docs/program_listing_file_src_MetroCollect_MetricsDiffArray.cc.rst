
.. _program_listing_file_src_MetroCollect_MetricsDiffArray.cc:

Program Listing for File MetricsDiffArray.cc
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_MetricsDiffArray.cc>` (``src/MetroCollect/MetricsDiffArray.cc``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // MetricsDiffArray.cc
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
   
   #include "MetricsDiffArray.h"
   
   
   namespace MetroCollect {
           MetricsDiffArray::MetricsDiffArray() noexcept : MetricsArray() { }
   
           MetricsDiffArray::MetricsDiffArray(MetricsSource::SourceInterests sourceInterests) noexcept : MetricsArray(sourceInterests) { }
   
   
           const std::chrono::system_clock::time_point& MetricsDiffArray::startTime() const noexcept {
                   return this->startTime_;
           }
   
           const std::chrono::system_clock::time_point& MetricsDiffArray::endTime() const noexcept {
                   return this->endTime_;
           }
   
           void MetricsDiffArray::computeDiff(MetricsArray<DataValueType>& current, MetricsArray<DataValueType>& previous, double factor) noexcept {
                   this->startTime_ = previous.timestamp();
                   this->endTime_ = current.timestamp();
                   this->timestamp_ = this->startTime_ + (this->endTime_ - this->startTime_) / 2;
   
                   this->sources_.forEach([&](auto& source, size_t sourceIndex) {
                           if ((*this->sourceInterests_)[sourceIndex].any())
                                   source.computeDiff((*this->sourceInterests_)[sourceIndex], this->sourceIterators_[sourceIndex].first,
                                                                           static_cast<typename MetricsArray<DataValueType>::ConstIterator>(current.sourceIterators_[sourceIndex].first),
                                                                           static_cast<typename MetricsArray<DataValueType>::ConstIterator>(previous.sourceIterators_[sourceIndex].first),
                                                                           factor);
                   });
           }
   }
