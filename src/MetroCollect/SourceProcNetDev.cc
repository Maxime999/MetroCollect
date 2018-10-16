//
// SourceProcNetDev.cc
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

#include <algorithm>
#include <cstring>

#include "SourceProcNetDev.h"


namespace MetroCollect::MetricsSource {
	SourceProcNetDev::SourceProcNetDev() {
		resetFile(this->file_, this->buffer_, SourceProcNetDev::filePath);
		this->parseFields();
	}


	void SourceProcNetDev::parseFields() {
		bool read = readFile(this->file_, this->buffer_, SourceProcNetDev::filePath);
		if (!read)
			return;

		const char* buffer = std::strchr(this->buffer_.data(), '\n') + 1;
		buffer = std::strchr(buffer, '|') + 1;

		this->rxFieldNames_.clear();
		while(buffer[0] != '|') {
			while (!std::isalnum(buffer[0]))
				buffer++;
			size_t length = 0;
			while (std::isalnum(buffer[length]))
				length++;
			this->rxFieldNames_.emplace_back(buffer, length);
			buffer += length;
		}
		buffer++;

		this->txFieldNames_.clear();
		while(buffer[0] != '\n') {
			while (!std::isalnum(buffer[0]))
				buffer++;
			size_t length = 0;
			while (std::isalnum(buffer[length]))
				length++;
			this->txFieldNames_.emplace_back(buffer, length);
			buffer += length;
		}
		buffer++;

		this->interfaces_.clear();
		while (buffer[0] != '\0') {
			while (!std::isalnum(buffer[0]))
				buffer++;
			size_t length = 0;
			while (std::isalnum(buffer[length]))
				length++;
			this->interfaces_.emplace_back(buffer, length);
			buffer += length;
			while (buffer[0] != '\n')
				buffer++;
			buffer++;
		}
	}


	size_t SourceProcNetDev::fieldCount() const noexcept {
		return this->interfaces_.size() * (this->rxFieldNames_.size() + this->txFieldNames_.size());
	}

	const std::vector<size_t> SourceProcNetDev::indexesOfFieldName(const FieldName& fieldName, Interests* interests) const noexcept {
		if (fieldName.front() != SourceProcNetDev::sourcePrefix)
			return {};

		size_t baseIndex = -1;
		if (fieldName[2] == SourceProcNetDev::fieldRecieve) {
			auto itr = std::find(this->rxFieldNames_.begin(), this->rxFieldNames_.end(), fieldName[3]);
			if (itr != this->rxFieldNames_.end()) {
				if (interests)
					interests->set(0);
				baseIndex = std::distance(this->rxFieldNames_.begin(), itr);
			}
		}
		else if (fieldName[2] == SourceProcNetDev::fieldTransmit) {
			auto itr = std::find(this->txFieldNames_.begin(), this->txFieldNames_.end(), fieldName[3]);
			if (itr != this->txFieldNames_.end()) {
				if (interests)
					interests->set(0);
				baseIndex = this->rxFieldNames_.size() + std::distance(this->txFieldNames_.begin(), itr);
			}
		} else
			return {};

		if (baseIndex < this->rxFieldNames_.size() + this->txFieldNames_.size()) {
			if (fieldName[1] == fieldNameAll) {
				std::vector<size_t> indexes;
				indexes.resize(this->interfaces_.size(), 0);
				for (size_t i = 0; i < indexes.size(); i++)
					indexes[i] = i * (this->rxFieldNames_.size() + this->txFieldNames_.size()) + baseIndex;
				return indexes;
			}
			auto itr = std::find(this->interfaces_.begin(), this->interfaces_.end(), fieldName[1]);
			if (itr != this->interfaces_.end())
				return {std::distance(this->interfaces_.begin(), itr) * (this->rxFieldNames_.size() + this->txFieldNames_.size()) + baseIndex};
		}

		return {};
	}

	const std::string SourceProcNetDev::fieldNameSourcePrefix() const noexcept {
		return std::string(SourceProcNetDev::sourcePrefix);
	}

	const FieldInfo SourceProcNetDev::fieldInfoAtIndex(size_t index) const noexcept {
		size_t ifIndex = index / (this->rxFieldNames_.size() + this->txFieldNames_.size());
		size_t fieldIndex = index % (this->rxFieldNames_.size() + this->txFieldNames_.size());
		bool isRx = fieldIndex < this->rxFieldNames_.size();
		if (!isRx)
			fieldIndex -= this->rxFieldNames_.size();
		std::string fieldKind = (isRx ? std::string(SourceProcNetDev::fieldRecieve) : std::string(SourceProcNetDev::fieldTransmit));
		std::string fieldName = (isRx ? this->rxFieldNames_[fieldIndex] : this->txFieldNames_[fieldIndex]);
		FieldName name = {std::string(SourceProcNetDev::sourcePrefix), this->interfaces_[ifIndex], fieldKind, fieldName};
		std::string unit = findUnit(fieldName, SourceProcNetDev::fieldUnitsAssociation, SourceProcNetDev::defaultUnit);
		return {name, "Network interface " + this->interfaces_[ifIndex] + " metric: " + fieldKind + "/" + fieldName, unit, 1, std::string(SourceProcNetDev::fieldNameInterfaceDescription)};
	}

	const std::vector<FieldInfo> SourceProcNetDev::allFieldsInfo() const noexcept {
		std::vector<FieldInfo> info;
		for (size_t i = 0; i < this->rxFieldNames_.size() + this->txFieldNames_.size(); i++) {
			info.push_back(this->fieldInfoAtIndex(i));
			info.back().name[info.back().dynamicIndexes[0].index] = SourceProcNetDev::fieldNameAll;
		}
		return info;
	}


	void SourceProcNetDev::fetchData(const Interests& interests, DataArray::Iterator current) {
		bool read = false;
		if (interests.none() || !(read = readFile(this->file_, this->buffer_, SourceProcNetDev::filePath))) {
			std::fill_n(current, this->fieldCount(), 0);
			return;
		}

		size_t remainingInterfaces = this->interfaces_.size();
		const char* buffer = this->buffer_.data();
		while (buffer[0] != '\n')
			buffer++;
		buffer++;
		while (buffer[0] != '\n')
			buffer++;
		buffer++;

		while (remainingInterfaces > 0 && buffer[0] != '\0') {
			while (buffer[0] != ':')
				buffer++;
			buffer++;
			while (buffer[0] != '\n') {
				buffer++;
				*current = static_cast<DataValueType>(parseUint(buffer));
				current++;
			}
			buffer++;
			remainingInterfaces--;
		}
	}


	void SourceProcNetDev::computeDiff(const Interests& interests, DiffArray::Iterator diff, DataArray::ConstIterator current, DataArray::ConstIterator previous, double factor) noexcept {
		if (interests.none())
			return;

		for (size_t i = 0; i < this->fieldCount(); i++) {
			*diff = static_cast<DiffValueType>((*current - *previous) * factor);
			diff++;
			current++;
			previous++;
		}
	}
}
