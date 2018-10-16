//
// CircularArray.cc
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

// Template file to be included in header

#ifndef CIRCULAR_ARRAY_CC
#define CIRCULAR_ARRAY_CC

#include "CircularArray.h"


namespace CircularArray {
	template<typename T>
	CircularArray<T>::CircularArray() { }

	template<typename T>
	CircularArray<T>::CircularArray(size_t n) : data_(n), capacity_(n), begin_(0), size_(0) { }

	template<typename T>
	CircularArray<T>::CircularArray(size_t n, const T& value) : data_(n, value), capacity_(n), begin_(0), size_(0) { }


	template<typename T>
	size_t CircularArray<T>::capacity() const noexcept {
		return this->capacity_;
	}

	template<typename T>
	size_t CircularArray<T>::size() const noexcept {
		return this->size_;
	}


	template<typename T>
	T& CircularArray<T>::at(ptrdiff_t index) {
		return this->data_.at(this->absoluteIndex(index));
	}

	template<typename T>
	const T& CircularArray<T>::at(ptrdiff_t index) const {
		return this->data_.at(this->absoluteIndex(index));
	}

	template<typename T>
	T& CircularArray<T>::operator[](ptrdiff_t index) {
		return this->data_[this->absoluteIndex(index)];
	}

	template<typename T>
	const T& CircularArray<T>::operator[](ptrdiff_t index) const {
		return this->data_[this->absoluteIndex(index)];
	}


	template<typename T>
	size_t CircularArray<T>::absoluteIndex(ptrdiff_t index) const noexcept {
		return ((this->begin_ + index) % this->capacity_ + this->capacity_) % this->capacity_;
	}

	template<typename T>
	T& CircularArray<T>::atAbsoluteIndex(size_t index)  {
		return this->data_[index];
	}

	template<typename T>
	const T& CircularArray<T>::atAbsoluteIndex(size_t index) const  {
		return this->data_[index];
	}

	template<typename T>
	bool CircularArray<T>::absoluteIndexIsInBounds(size_t index) const noexcept {
		bool overflow = (this->begin_ + this->size_ > this->capacity_);
		bool larger = (this->begin_ <= index);
		bool smaller = (index <= (this->begin_ + this->size_ - 1) % this->capacity_);
		return (!overflow && larger && smaller) || (overflow && (larger || smaller));
	}


	template<typename T>
	T& CircularArray<T>::front() {
		return (*this)[0];
	}

	template<typename T>
	const T& CircularArray<T>::front() const {
		return (*this)[0];
	}

	template<typename T>
	T& CircularArray<T>::back() {
		return (*this)[this->size_ - 1];
	}

	template<typename T>
	const T& CircularArray<T>::back() const {
		return (*this)[this->size_ - 1];
	}


	template<typename T>
	void CircularArray<T>::moveBegin(ptrdiff_t indexes) noexcept{
		this->begin_ = this->absoluteIndex(indexes);
		if (indexes > static_cast<ptrdiff_t>(this->size_))
			this->size_ = 0;
		else if (indexes < -static_cast<ptrdiff_t>(this->capacity_ - this->size_))
			this->size_ = this->capacity_;
		else
			this->size_ -= indexes;
	}

	template<typename T>
	void CircularArray<T>::moveEnd(ptrdiff_t indexes) noexcept {
		if (indexes > static_cast<ptrdiff_t>(this->capacity_ - this->size_)) {
			this->begin_ = this->absoluteIndex(this->size_ + indexes);
			this->size_ = this->capacity_;
		} else if (indexes < -static_cast<ptrdiff_t>(this->size_)) {
			this->begin_ = this->absoluteIndex(this->size_ + indexes);
			this->size_ = 0;
		} else
			this->size_ += indexes;
	}


	template<typename T>
	void CircularArray<T>::reset() noexcept{
		this->size_ = 0;
		this->begin_ = 0;
	}

	template<typename T>
	void CircularArray<T>::reset(size_t capacity) noexcept {
		this->capacity_ = capacity;
		this->data_.resize(capacity);
		this->reset();
	}

	template<typename T>
	void CircularArray<T>::reset(size_t capacity, const T& value) noexcept {
		this->capacity_ = capacity;
		this->data_.clear();
		this->data_.resize(capacity, value);
		this->reset();
	}

	template<typename T>
	void CircularArray<T>::reset(const T& value) noexcept {
		this->data_.clear();
		this->data_.resize(this->capacity_, value);
		this->reset();
	}
}

#endif // CIRCULAR_ARRAY_CC
