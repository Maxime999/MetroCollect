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
	/**
	 * @brief Sources of network metrics extracted from Ethtool interface
	 */
	class SourceEthtool : public SourceBase {
		protected:
			static constexpr std::string_view sourcePrefix = "ethtool"sv;							//!< Metrics name source prefix
			static constexpr std::string_view fieldNameAll = "*"sv;									//!< Metrics name wildcard
			static constexpr std::string_view fieldNameInterfaceDescription = "interface"sv;		//!< Metrics name category description
			static constexpr std::string_view fieldNameIndexDescription = "index"sv;				//!< Metrics name category description
			static constexpr std::array fieldUnitsAssociation = { KeyUnit{"bytes"sv, "bytes"sv}, KeyUnit{"packets"sv, "packets"sv}, KeyUnit{"pkts"sv, "packets"sv} };		//!< Metric units associations
			static constexpr std::string_view defaultUnit = ""sv;									//!< Metric unit


			/**
			 * @brief Class managing a pointer to a dynamically sized type
			 *
			 * @tparam T The type of the object managed
			 * @tparam P Elementary memory unit, defaults to char (1 byte)
			 */
			template<typename T, typename P = char>
			class EthtoolPointer {
				public:
					using ValueType = T;		//!< The type of the object managed
					using PaddingType = P;		//!< Elementary memory unit

				protected:
					size_t paddingSize_;		//!< Size of added dynamic padding, in multiples of type P size
					T* pointer_ = nullptr;		//!< Managed pointer

				public:
					/**
					 * @brief Construct a new, empty Ethtool Pointer object
					 */
					EthtoolPointer() noexcept : pointer_(nullptr) { }

					/**
					 * @brief Construct a new Ethtool Pointer object
					 *
					 * @param cmd Ethtool cmd value
					 * @param padding Padding size to append after the object
					 */
					EthtoolPointer(uint32_t cmd, size_t padding = 0) noexcept {
						this->paddingSize_ = padding;
						this->pointer_ = static_cast<T*>(calloc(1, sizeof(T) + padding * sizeof(P)));
						this->pointer_->cmd = cmd;
					}

					/**
					 * @brief Move constructor
					 *
					 * @tparam Q other elementary memory unit
					 * @param other EthtoolPointer to move
					 */
					template<typename Q>
					EthtoolPointer(EthtoolPointer<T, Q>&& other) noexcept {
						*this = std::move(other);
					}

					/**
					 * @brief Deleted copy constructor
					 */
					EthtoolPointer(EthtoolPointer&) = delete;

					/**
					 * @brief Move assignment operator
					 *
					 * @tparam Q other elementary memory unit
					 * @param other Ethtool Pointer to move
					 * @return *this* with updated values
					 */
					template<typename Q>
					EthtoolPointer& operator=(EthtoolPointer<T, Q>&& other) noexcept {
						if (this->pointer_ != other.pointer_) {
							free(static_cast<void*>(this->pointer_));
							this->pointer_ = nullptr;
							std::swap(this->pointer_, other.pointer_);
						}
						return *this;
					}

					/**
					 * @brief Deleted copy assignment operator
					 */
					EthtoolPointer& operator=(EthtoolPointer&) = delete;

					/**
					 * @brief Destroy the Ethtool Pointer object
					 */
					~EthtoolPointer() {
						free(static_cast<void*>(this->pointer_));
					}


					/**
					 * @brief Dereferences pointer to the managed object
					 *
					 * @return The object owned by *this*
					 */
					T& operator*() {
						return *this->pointer_;
					}

					/**
					 * @brief Dereferences pointer to the managed object
					 *
					 * @return The const object owned by *this*
					 */
					const T& operator*() const {
						return *this->pointer_;
					}

					/**
					 * @brief Dereferences pointer to the managed object
					 *
					 * @return A pointer to the object owned by *this*
					 */
					T* operator->() noexcept {
						return this->pointer_;
					}

					/**
					 * @brief Dereferences pointer to the managed object
					 *
					 * @return A const pointer to the object owned by *this*
					 */
					const T* operator->() const noexcept {
						return this->pointer_;
					}


					/**
					 * @brief Padding size appended after the object
					 *
					 * @return The padding size
					 */
					size_t paddingSize() const noexcept {
						return this->paddingSize_;
					}

					/**
					 * @brief Total size of the managed object
					 *
					 * @return size_t
					 */
					size_t totalSizeInBytes() const noexcept {
						return sizeof(T) + this->paddingSize_ * sizeof(P);
					}

					/**
					 * @brief Returns the raw managed pointer with Ethtool-specific type
					 *
					 * @return The raw managed pointer
					 */
					inline caddr_t raw() const noexcept {
						return reinterpret_cast<caddr_t>(this->pointer_);
					}
			};


			/**
			 * @brief Class to store the details and metrics names of a network interface
			 */
			struct InterfaceInfo {
				/**
				 * @brief Name of a metric
				 */
				struct NameAndIndex {
					std::string name;						//!< Name of a metric
					std::optional<std::string> index;		//!< index of the metric if the metric name has an index
				};

				std::string name;							//!< Name of the interface
				std::string driver;							//!< Driver name of the interface
				size_t fieldCount;							//!< Number of metrics of the interface
				std::vector<NameAndIndex> fieldNames;		//!< Names of the metrics
				std::vector<size_t> fieldIndexes;			//!< Indexes of relevant metrics (some may be skipped)
				struct ifreq ifr;							//!< Ethtool data structure of this interface

				EthtoolPointer<struct ethtool_stats, uint64_t> statsValues;		//!< Ethtool statistics array

				explicit InterfaceInfo(const char* aName);	//!< Construct a new Interface Info object

				InterfaceInfo() = delete;					//!< Deleted default constructor
			};


			/**
			 * @brief Struct managing an Ethtool socket
			 */
			struct Socketfd {
				int fd;													//!< Socket descriptor

				Socketfd();												//!< Construct a new Socketfd object
				~Socketfd();											//!< Destroy the Socketfd object
				Socketfd(const Socketfd&) = delete;						//!< Deleted copy constructor
				Socketfd& operator=(const Socketfd&) = delete;			//!< Delete assignment operator
			};

			std::vector<InterfaceInfo> ifInfo_;				//!< Array holding details of each network interface
			Socketfd socketfd_;								//!< Ethtool socket manager

			SourceEthtool();								//!< Private default constructor
			void gatherIfData();							//!< Gather details on interfaces
			InterfaceInfo::NameAndIndex parseEthtoolString(std::string ethtoolString);		//!< Parse raw Ethtool metric name
			void ixgbeIfFilter(InterfaceInfo& ifInfo);		//!< Filter out relevant metrics for ixgbe driver

		public:
			/**
			 * @brief Get the Instance object
			 */
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
