//
// SourceEthtool.cc
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

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <numeric>

#include "SourceEthtool.h"


namespace MetroCollect::MetricsSource {
	SourceEthtool::InterfaceInfo::InterfaceInfo(const char* aName) :
		name(aName),
		fieldCount(0)
	{
		strncpy(this->ifr.ifr_name, aName, IF_NAMESIZE);
	}


	SourceEthtool::Socketfd::Socketfd() {
		errno = 0;
		this->fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (errno) {
			perror("Ethtool: Error opening socket");
			errno = 0;
		}
	}

	SourceEthtool::Socketfd::~Socketfd() {
		int result = 0;
		errno = 0;
		do {
			result = close(this->fd);
		} while (result == -1 && errno == EINTR);
		if (errno) {
			perror("Ethtool: Error closing socket");
			errno = 0;
		}
	}


	SourceEthtool::SourceEthtool() {
		gatherIfData();
	}


	void SourceEthtool::gatherIfData() {
		struct ifreq ifr;

		this->ifInfo_.clear();

		while (true) {
			ifr.ifr_ifindex = this->ifInfo_.size() + 1;
			int val = ioctl(this->socketfd_.fd, SIOCGIFNAME, &ifr);
			if (val != 0)
				break;
			ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
			this->ifInfo_.emplace_back(ifr.ifr_name);
		}
		errno = 0;

		auto driverInfo = EthtoolPointer<struct ethtool_drvinfo>(ETHTOOL_GDRVINFO);
		auto ssetInfo = EthtoolPointer<struct ethtool_sset_info, uint32_t>(ETHTOOL_GSSET_INFO, 1);
		for (size_t i = 0; i < this->ifInfo_.size(); i++) {
			this->ifInfo_[i].ifr.ifr_data = driverInfo.raw();
			if (ioctl(this->socketfd_.fd, SIOCETHTOOL, &this->ifInfo_[i].ifr) == 0)
				this->ifInfo_[i].driver = driverInfo->driver;
			else {
				this->ifInfo_[i].driver = "unknown";
				errno = 0;
			}
			ssetInfo->sset_mask = 1 << ETH_SS_STATS;
			this->ifInfo_[i].ifr.ifr_data = ssetInfo.raw();
			if (ioctl(this->socketfd_.fd, SIOCETHTOOL, &this->ifInfo_[i].ifr) == 0) {
				size_t count = ssetInfo->sset_mask ? ssetInfo->data[0] : 0;
				this->ifInfo_[i].fieldCount = count;
				this->ifInfo_[i].statsValues = EthtoolPointer<struct ethtool_stats, uint64_t>(ETHTOOL_GSTATS, count);
				if (count == 0)
					continue;

				auto gstrings = EthtoolPointer<struct ethtool_gstrings, char[ETH_GSTRING_LEN]>(ETHTOOL_GSTRINGS, count);
				gstrings->string_set = ETH_SS_STATS;
				gstrings->len = count;
				this->ifInfo_[i].ifr.ifr_data = gstrings.raw();
				if (ioctl(this->socketfd_.fd, SIOCETHTOOL, &this->ifInfo_[i].ifr) == 0) {
					for (size_t j = 0; j < gstrings->len; j++) {
						gstrings->data[(j + 1) * ETH_GSTRING_LEN - 1] = '\0';
						this->ifInfo_[i].fieldNames.emplace_back(this->parseEthtoolString(reinterpret_cast<char*>(&gstrings->data[j * ETH_GSTRING_LEN])));
					}
					this->ifInfo_[i].fieldIndexes.resize(this->ifInfo_[i].fieldCount);
					std::iota(this->ifInfo_[i].fieldIndexes.begin(), this->ifInfo_[i].fieldIndexes.end(), 0);
					if (this->ifInfo_[i].driver == "ixgbe")
						this->ixgbeIfFilter(this->ifInfo_[i]);
				} else {
					perror("Ethtool: cannot read device stats strings");
					errno = 0;
				}
			} else {
				perror("Ethtool: cannot read device stats count");
				errno = 0;
			}
			this->ifInfo_[i].ifr.ifr_data = nullptr;
		}
	}

	SourceEthtool::InterfaceInfo::NameAndIndex SourceEthtool::parseEthtoolString(std::string ethtoolString) {
		std::string indexString = "";
		size_t index = ethtoolString.find_first_of("0123456789");
		if (index != std::string::npos) {
			size_t count = 0;
			while (std::isdigit(ethtoolString[index + count])) {
				indexString.append(ethtoolString, index + count, 1);
				count++;
			}
			if (index + count >= ethtoolString.size())
				indexString.clear();
			else
				ethtoolString.replace(index, count, "i");
		}

		bool wasCapitalized = false;
		for (size_t i = 0; i < ethtoolString.size(); i++) {
			if (!std::isalnum(ethtoolString[i]) && ethtoolString[i] != '_')
				ethtoolString[i] = '_';
			else if (std::isupper(ethtoolString[i])) {
				if (i > 0 && ethtoolString[i - 1] != '_' && std::islower(ethtoolString[i - 1]) && !wasCapitalized) {
					ethtoolString.insert(i, "_");
					i++;
				}
				ethtoolString[i] = std::tolower(ethtoolString[i]);
				wasCapitalized = true;
			} else
				wasCapitalized = false;
		}
		while (ethtoolString.back() == '_')
			ethtoolString.erase(ethtoolString.size() - 1);
		while (ethtoolString.front() == '_')
			ethtoolString.erase(0);

		InterfaceInfo::NameAndIndex ret;
		ret.name = std::move(ethtoolString);
		if (!indexString.empty())
			ret.index =std::move(indexString);
		return ret;
	}

	void SourceEthtool::ixgbeIfFilter(InterfaceInfo& ifInfo) {
		auto channels = EthtoolPointer<struct ethtool_channels>(ETHTOOL_GCHANNELS);
		ifInfo.ifr.ifr_data = channels.raw();
		if (ioctl(this->socketfd_.fd, SIOCETHTOOL, &ifInfo.ifr) == 0) {
			ifInfo.fieldCount = ifInfo.fieldNames.size();
			ifInfo.fieldIndexes.clear();
			for (size_t i = 0; i < ifInfo.fieldNames.size(); i++) {
				const char* name = ifInfo.fieldNames[i].name.c_str();
				if ((name[0] == 'r' || name[0] == 't') && std::strncmp(name + 1, "x_queue_i_", 10) == 0 && ifInfo.fieldNames[i].index.has_value() && std::stoull(ifInfo.fieldNames[i].index.value()) >= channels->combined_count)
					ifInfo.fieldCount--;
				else
					ifInfo.fieldIndexes.push_back(i);
			}
		} else {
			perror("Ethtool: cannot read device channels");
			errno = 0;
		}
		ifInfo.ifr.ifr_data = nullptr;
	}


	size_t SourceEthtool::fieldCount() const noexcept {
		size_t count = 0;
		for (const auto& info : this->ifInfo_)
			count += info.fieldCount;
		return count;
	}

	const std::vector<size_t> SourceEthtool::indexesOfFieldName(const FieldName& fieldName, Interests* interests) const noexcept {
		if (fieldName[0] != SourceEthtool::sourcePrefix)
			return {};

		size_t baseIndex = 0;
		std::vector<size_t> indexes;
		for (size_t i = 0; i < this->ifInfo_.size(); i++) {
			if (fieldName[2] == this->ifInfo_[i].name || (fieldName[1] == this->ifInfo_[i].driver && fieldName[2] == SourceEthtool::fieldNameAll)) {
				auto itr = std::find_if(this->ifInfo_[i].fieldIndexes.begin(), this->ifInfo_[i].fieldIndexes.end(), [&](const auto& a) {
					return this->ifInfo_[i].fieldNames[a].name == fieldName[3];
				});
				while (itr != this->ifInfo_[i].fieldIndexes.end()) {
					indexes.push_back(std::distance(this->ifInfo_[i].fieldIndexes.begin(), itr) + baseIndex);
					if (interests)
						interests->set(i);
					itr++;
					itr = std::find_if(itr, this->ifInfo_[i].fieldIndexes.end(), [&](const auto& a) {
						return this->ifInfo_[i].fieldNames[a].name == fieldName[3];
					});
				}
			}
			baseIndex += this->ifInfo_[i].fieldCount;
		}

		return indexes;
	}

	const std::string SourceEthtool::fieldNameSourcePrefix() const noexcept {
		return std::string(SourceEthtool::sourcePrefix);
	}

	const FieldInfo SourceEthtool::fieldInfoAtIndex(size_t index) const noexcept {
		for (size_t i = 0; i < this->ifInfo_.size(); i++) {
			if (index < this->ifInfo_[i].fieldCount) {
				auto& fieldName = this->ifInfo_[i].fieldNames[this->ifInfo_[i].fieldIndexes[index]];
				FieldName name = {std::string(SourceEthtool::sourcePrefix), this->ifInfo_[i].driver, this->ifInfo_[i].name, fieldName.name};
				if (fieldName.index)
					name.push_back(fieldName.index.value());
				std::string unit = findUnit(fieldName.name, SourceEthtool::fieldUnitsAssociation, SourceEthtool::defaultUnit);
				FieldInfo info = {name, "Interface " + this->ifInfo_[i].name + " metric: " + fieldName.name, unit, 2, std::string(SourceEthtool::fieldNameInterfaceDescription)};
				if (fieldName.index)
					info.dynamicIndexes.emplace_back(4, std::string(SourceEthtool::fieldNameIndexDescription));
				return info;
			} else
				index -= this->ifInfo_[i].fieldCount;
		}
		return {};
	}

	const std::vector<FieldInfo> SourceEthtool::allFieldsInfo() const noexcept {
		std::vector<FieldInfo> info;
		size_t count = this->fieldCount();
		for (size_t i = 0; i < count; i++)
			info.push_back(this->fieldInfoAtIndex(i));

		std::sort(info.begin(), info.end(), [&](const auto& a, const auto& b) {
			auto cmp = alphanumCompare(a.name[1].c_str(), b.name[1].c_str());
			if (cmp < 0)
				return true;
			if (cmp == 0) {
				cmp = alphanumCompare(a.name[3].c_str(), b.name[3].c_str());
				if (cmp < 0)
					return true;
				if (cmp == 0 && a.name.size() > 4 && b.name.size() > 4) {
					cmp = alphanumCompare(a.name[4].c_str(), b.name[4].c_str());
					if (cmp < 0)
						return true;
				}
			}
			return false;
		});
		auto last = std::unique(info.begin(), info.end(), [&](const auto& a, const auto& b) {
			return (a.name[1] == b.name[1] && a.name[3] == b.name[3]);
		});
		info.erase(last, info.end());

		for (auto& field : info) {
			for (const auto& dynamicIndex: field.dynamicIndexes)
				field.name[dynamicIndex.index] = std::string(SourceEthtool::fieldNameAll);
		}

		return info;
	}


	void SourceEthtool::fetchData(const Interests& interests, DataArray::Iterator current) {
		for (size_t i = 0; i < this->ifInfo_.size(); i++) {
			if (this->ifInfo_[i].fieldCount == 0)
				continue;
			else if (!interests.isSet(i)) {
				std::fill_n(current, this->ifInfo_[i].fieldCount, 0);
				current += this->ifInfo_[i].fieldCount;
				continue;
			}

			this->ifInfo_[i].ifr.ifr_data = this->ifInfo_[i].statsValues.raw();
			if (ioctl(this->socketfd_.fd, SIOCETHTOOL, &this->ifInfo_[i].ifr) == 0) {
				for (size_t j: this->ifInfo_[i].fieldIndexes) {
					*current = static_cast<DataValueType>(this->ifInfo_[i].statsValues->data[j]);
					current++;
				}
			} else {
				perror("Ethtool: cannot read device stats values");
				errno = 0;
				std::fill_n(current, this->ifInfo_[i].fieldCount, 0);
			}
			this->ifInfo_[i].ifr.ifr_data = nullptr;
		}
	}


	void SourceEthtool::computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor) noexcept {
		for (size_t i = 0; i < this->ifInfo_.size(); i++) {
			if (!interests.isSet(i))
				continue;
			for (size_t j = 0; j < this->ifInfo_[i].fieldCount; j++) {
				*diff = static_cast<DiffValueType>((*current - *previous) * factor);
				diff++;
				current++;
				previous++;
			}
		}
	}
}
