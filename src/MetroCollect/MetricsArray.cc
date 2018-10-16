//
// MetricsArray.cc
//
// Created on July 23rd 2018
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

#include "MetricsArray.h"
#include "Statistics.h"

using namespace std::literals;


namespace MetroCollect {
	template<typename T>
	MetricsArray<T>::MetricsArray() noexcept : sourceInterests_(MetricsSource::makeSourceInterests()) {
		this->setSize();
	}

	template<typename T>
	MetricsArray<T>::MetricsArray(MetricsSource::SourceInterests sourceInterests) noexcept : sourceInterests_(sourceInterests) {
		this->setSize();
	}


	template<typename T>
	MetricsArray<T>::MetricsArray(const MetricsArray& other) noexcept : sourceInterests_(other.sourceInterests_), timestamp_(other.timestamp_), data_(other.data_) {
		this->setIterators();
	}

	template<typename T>
	MetricsArray<T>& MetricsArray<T>::operator=(const MetricsArray& other) noexcept {
		this->sourceInterests_ = other.sourceInterests_;
		this->timestamp_ = other.timestamp_;
		this->data_ = other.data_;
		this->setIterators();
		return *this;
	}


	template<typename T>
	MetricsSource::SourceInterests MetricsArray<T>::sourceInterests() const noexcept {
		return this->sourceInterests_;
	}

	template<typename T>
	void MetricsArray<T>::setSourcesInterests(MetricsSource::SourceInterests sourceInterests) noexcept {
		this->sourceInterests_ = sourceInterests;
	}


	template<typename T>
	const std::chrono::system_clock::time_point& MetricsArray<T>::timestamp() const noexcept {
		return this->timestamp_;
	}


	template<typename T>
	size_t MetricsArray<T>::fieldCount() const noexcept {
		size_t size = 0;

		this->sources_.forEach([&](auto& source, size_t ) {
			size += source.fieldCount();
		});

		return size;
	}

	template<typename T>
	const std::vector<size_t> MetricsArray<T>::indexesOfFieldName(const MetricsSource::FieldName& fieldName, bool setInterest) const noexcept {
		std::vector<size_t> indexes;
		size_t baseIndex = 0;

		this->sources_.forEach([&](auto& source, size_t sourceIndex) {
			if (fieldName.front() == source.fieldNameSourcePrefix()) {
				MetricsSource::Interests* interests = (setInterest ? &(*this->sourceInterests_)[sourceIndex] : nullptr);
				indexes = source.indexesOfFieldName(fieldName, interests);
				for (auto& i : indexes)
					i += baseIndex;
			} else
				baseIndex += std::distance(this->sourceIterators_[sourceIndex].first, this->sourceIterators_[sourceIndex].second);
		});

		return indexes;
	}

	template<typename T>
	const MetricsSource::FieldInfo MetricsArray<T>::fieldInfoAtIndex(size_t index) const noexcept {
		MetricsSource::FieldInfo info;
		bool stop = false;

		this->sources_.forEach([&](auto& source, size_t sourceIndex) {
			if (stop)
				return;
			if (index < source.fieldCount()) {
				info = source.fieldInfoAtIndex(index);
				stop = true;
			}
			else
				index -= std::distance(this->sourceIterators_[sourceIndex].first, this->sourceIterators_[sourceIndex].second);
		});

		return info;
	}

	template<typename T>
	const std::vector<MetricsSource::FieldInfo> MetricsArray<T>::allFieldsInfo() const noexcept {
		std::vector<MetricsSource::FieldInfo> allInfo;

		this->sources_.forEach([&](auto& source, size_t ) {
			std::vector<MetricsSource::FieldInfo> info = source.allFieldsInfo();
			std::move(info.begin(), info.end(), std::back_inserter(allInfo));
		});

		return allInfo;
	}


	template<typename T>
	size_t MetricsArray<T>::size() const noexcept {
		return this->data_.size();
	}

	template<typename T>
	T& MetricsArray<T>::at(size_t index) {
		return this->data_.at(index);
	}

	template<typename T>
	const T& MetricsArray<T>::at(size_t index) const {
		return this->data_.at(index);
	}


	template<typename T>
	std::pair<typename MetricsArray<T>::Iterator, typename MetricsArray<T>::Iterator> MetricsArray<T>::metricsSliceForSource(size_t sourceIndex) {
		return this->sourceIterators_[sourceIndex];
	}


	template<typename T>
	typename MetricsArray<T>::Iterator MetricsArray<T>::begin() noexcept {
		return this->data_.begin();
	}

	template<typename T>
	const typename MetricsArray<T>::ConstIterator MetricsArray<T>::begin() const noexcept {
		return this->data_.begin();
	}

	template<typename T>
	typename MetricsArray<T>::Iterator MetricsArray<T>::end() noexcept {
		return this->data_.end();
	}

	template<typename T>
	const typename MetricsArray<T>::ConstIterator MetricsArray<T>::end() const noexcept {
		return this->data_.end();
	}


	template<typename T>
	void MetricsArray<T>::setSize() noexcept {
		size_t size = this->fieldCount();
		this->data_.resize(size);
		this->setIterators();
	}

	template<typename T>
	void MetricsArray<T>::setIterators() noexcept {
		auto itr = this->data_.begin();
		this->sources_.forEach([&](auto& source, size_t sourceIndex) {
			this->sourceIterators_[sourceIndex].first = itr;
			itr += source.fieldCount();
			this->sourceIterators_[sourceIndex].second = itr;
		});
	}


	template class MetricsArray<DataValueType>;
	template class MetricsArray<DiffValueType>;
	template class MetricsArray<size_t>;
	template class MetricsArray<Statistics::Stats>;
}
