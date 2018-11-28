
.. _program_listing_file_src_CircularArray_CircularArray.h:

Program Listing for File CircularArray.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_CircularArray_CircularArray.h>` (``src/CircularArray/CircularArray.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // CircularArray.h
   //
   // Created on July 24th 2018
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
   
   #ifndef CIRCULAR_ARRAY_H
   #define CIRCULAR_ARRAY_H
   
   #include <vector>
   
   
   namespace CircularArray {
           template<typename T>
           class CircularArray {
                   protected:
                           std::vector<T> data_;   
                           size_t capacity_;               
                           size_t begin_;                  
                           size_t size_;                   
   
                   public:
                           using ValueType = T;    
   
   
                           CircularArray();
   
                           CircularArray(size_t n);
   
                           CircularArray(size_t n, const T& value);
   
   
                           size_t capacity() const noexcept;
   
                           size_t size() const noexcept;
   
   
                           T& at(ptrdiff_t index);
   
                           const T& at(ptrdiff_t index) const;
   
                           T& operator[](ptrdiff_t index);
   
                           const T& operator[](ptrdiff_t index) const;
   
   
                           size_t absoluteIndex(ptrdiff_t index) const noexcept;
   
                           T& atAbsoluteIndex(size_t index);
   
                           const T& atAbsoluteIndex(size_t index) const;
   
                           bool absoluteIndexIsInBounds(size_t index) const noexcept;
   
   
                           T& front();
   
                           const T& front() const;
   
                           T& back();
   
                           const T& back() const;
   
   
                           void moveBegin(ptrdiff_t indexes) noexcept;
   
                           void moveEnd(ptrdiff_t indexes) noexcept;
   
   
                           void reset() noexcept;
   
                           void reset(size_t capacity) noexcept;
   
                           void reset(size_t capacity, const T& value) noexcept;
   
                           void reset(const T& value) noexcept;
           };
   }
   
   #include "CircularArray.cc"
   
   #endif // CIRCULAR_ARRAY_H
