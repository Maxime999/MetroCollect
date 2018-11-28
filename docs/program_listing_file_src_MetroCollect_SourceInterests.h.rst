
.. _program_listing_file_src_MetroCollect_SourceInterests.h:

Program Listing for File SourceInterests.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceInterests.h>` (``src/MetroCollect/SourceInterests.h``)

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
   
   #include <memory>
   #include <vector>
   
   
   namespace MetroCollect::MetricsSource {
           struct Interests {
                   uint64_t value;         
   
   
                   inline Interests() : value(0) { }
   
                   template<typename T>
                   inline Interests(T v) : value(v) { }
   
   
                   template<typename T>
                   inline Interests& operator=(const T v) {
                           value = v;
                           return *this;
                   }
   
   
                   inline bool any() const {
                           return value != 0;
                   }
   
   
                   inline bool none() const {
                           return value == 0;
                   }
   
   
                   template<typename T>
                   inline bool isSet(T index) const {
                           return (value & (static_cast<size_t>(1) << (index % (8 * sizeof(size_t))))) != 0;
                   }
   
   
                   template<typename T>
                   inline void set(T index) {
                           value |= (static_cast<size_t>(1) << (index % (8 * sizeof(size_t))));
                   }
   
   
                   template<typename T>
                   inline void unSet(T index) {
                           value &= ~(static_cast<size_t>(1) << (index % (8 * sizeof(size_t))));
                   }
           };
   
   
           template<>
           inline Interests::Interests(bool v) : value(v ? ~0 : 0) { }
   
   
           template<>
           inline Interests& Interests::operator=(const bool v) {
                   value = (v ? ~0 : 0);
                   return *this;
           }
   
   
           using SourceInterests = std::shared_ptr<std::vector<MetricsSource::Interests>>;         
   
           SourceInterests makeSourceInterests(bool value = false);
   }
