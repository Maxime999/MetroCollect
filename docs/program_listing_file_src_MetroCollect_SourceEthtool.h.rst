
.. _program_listing_file_src_MetroCollect_SourceEthtool.h:

Program Listing for File SourceEthtool.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceEthtool.h>` (``src/MetroCollect/SourceEthtool.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // SourceEthtool.h
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
   
   #include <net/if.h>
   
   #include <array>
   #include <optional>
   #include <string>
   #include <string_view>
   #include <vector>
   
   #include "Source.h"
   
   using namespace std::literals;
   
   
   namespace MetroCollect::MetricsSource {
           class SourceEthtool : public SourceBase {
                   protected:
                           static constexpr std::string_view sourcePrefix = "ethtool"sv;                                                   
                           static constexpr std::string_view fieldNameAll = "*"sv;                                                                 
                           static constexpr std::string_view fieldNameInterfaceDescription = "interface"sv;                
                           static constexpr std::string_view fieldNameIndexDescription = "index"sv;                                
                           static constexpr std::array fieldUnitsAssociation = { KeyUnit{"bytes"sv, "bytes"sv}, KeyUnit{"packets"sv, "packets"sv}, KeyUnit{"pkts"sv, "packets"sv} };               
                           static constexpr std::string_view defaultUnit = ""sv;                                                                   
   
   
                           template<typename T, typename P = char>
                           class EthtoolPointer {
                                   public:
                                           using ValueType = T;            
                                           using PaddingType = P;          
   
                                   protected:
                                           size_t paddingSize_;            
                                           T* pointer_ = nullptr;          
   
                                   public:
                                           EthtoolPointer() noexcept : pointer_(nullptr) { }
   
                                           EthtoolPointer(uint32_t cmd, size_t padding = 0) noexcept {
                                                   this->paddingSize_ = padding;
                                                   this->pointer_ = static_cast<T*>(calloc(1, sizeof(T) + padding * sizeof(P)));
                                                   this->pointer_->cmd = cmd;
                                           }
   
                                           template<typename Q>
                                           EthtoolPointer(EthtoolPointer<T, Q>&& other) noexcept {
                                                   *this = std::move(other);
                                           }
   
                                           EthtoolPointer(EthtoolPointer&) = delete;
   
                                           template<typename Q>
                                           EthtoolPointer& operator=(EthtoolPointer<T, Q>&& other) noexcept {
                                                   if (this->pointer_ != other.pointer_) {
                                                           free(static_cast<void*>(this->pointer_));
                                                           this->pointer_ = nullptr;
                                                           std::swap(this->pointer_, other.pointer_);
                                                   }
                                                   return *this;
                                           }
   
                                           EthtoolPointer& operator=(EthtoolPointer&) = delete;
   
                                           ~EthtoolPointer() {
                                                   free(static_cast<void*>(this->pointer_));
                                           }
   
   
                                           T& operator*() {
                                                   return *this->pointer_;
                                           }
   
                                           const T& operator*() const {
                                                   return *this->pointer_;
                                           }
   
                                           T* operator->() noexcept {
                                                   return this->pointer_;
                                           }
   
                                           const T* operator->() const noexcept {
                                                   return this->pointer_;
                                           }
   
   
                                           size_t paddingSize() const noexcept {
                                                   return this->paddingSize_;
                                           }
   
                                           size_t totalSizeInBytes() const noexcept {
                                                   return sizeof(T) + this->paddingSize_ * sizeof(P);
                                           }
   
                                           inline caddr_t raw() const noexcept {
                                                   return reinterpret_cast<caddr_t>(this->pointer_);
                                           }
                           };
   
   
                           struct InterfaceInfo {
                                   struct NameAndIndex {
                                           std::string name;                                               
                                           std::optional<std::string> index;               
                                   };
   
                                   std::string name;                                                       
                                   std::string driver;                                                     
                                   size_t fieldCount;                                                      
                                   std::vector<NameAndIndex> fieldNames;           
                                   std::vector<size_t> fieldIndexes;                       
                                   struct ifreq ifr;                                                       
   
                                   EthtoolPointer<struct ethtool_stats, uint64_t> statsValues;             
   
                                   explicit InterfaceInfo(const char* aName);      
   
                                   InterfaceInfo() = delete;                                       
                           };
   
   
                           struct Socketfd {
                                   int fd;                                                                                                 
   
                                   Socketfd();                                                                                             
                                   ~Socketfd();                                                                                    
                                   Socketfd(const Socketfd&) = delete;                                             
                                   Socketfd& operator=(const Socketfd&) = delete;                  
                           };
   
                           std::vector<InterfaceInfo> ifInfo_;                             
                           Socketfd socketfd_;                                                             
   
                           SourceEthtool();                                                                
                           void gatherIfData();                                                    
                           InterfaceInfo::NameAndIndex parseEthtoolString(std::string ethtoolString);              
                           void ixgbeIfFilter(InterfaceInfo& ifInfo);              
   
                   public:
                           static SourceEthtool& get() {
                                   static SourceEthtool instance;
                                   return instance;
                           }
   
                           size_t fieldCount() const noexcept override final;
                           const std::vector<size_t> indexesOfFieldName(const FieldName& fieldName, Interests* interests = nullptr) const noexcept override final;
                           const std::string fieldNameSourcePrefix() const noexcept override final;
                           const FieldInfo fieldInfoAtIndex(size_t index) const noexcept override final;
                           const std::vector<FieldInfo> allFieldsInfo() const noexcept override final;
   
                           void fetchData(const Interests& interests, DataArray::Iterator current) override final;
                           void computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor = 1) noexcept override final;
           };
   }
