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
	/**
	 * @brief A circular array container
	 *
	 * A circular array is a container which maintains a range of active elements.
	 * The active range is defined by a starting index in the underlying container and a size. It may grow or shrink from both ends and users can access its elements.
	 * However, the active range size is limited by the underlying container capacity, in case of overflow elements at the beginning are replaced by elemnts added at the end.
	 *
	 * For a more thorough overview of circular buffers, refer to `boost::circular_buffer`'s documentation.
	 *
	 * In this implementation, elements may be accessed from the beginning of the active range in the usual way, but also from the beginning of the underlying container to maintain a consistent access.
	 *
	 * @tparam T Type of stored elements in the container
	 */
	template<typename T>
	class CircularArray {
		protected:
			std::vector<T> data_;	//!< Underlying data storage
			size_t capacity_;		//!< Number of elements that can be held in currently allocated storage
			size_t begin_;			//!< First index in internal storage
			size_t size_;			//!< Number of elements currently stored

		public:
			using ValueType = T;	//!< Type of stored elements in the container


			/**
			 * @brief Construct a new Circular Array object
			 */
			CircularArray();

			/**
			 * @brief Construct a new Circular Array object
			 *
			 * @param n size of the container
			 */
			CircularArray(size_t n);

			/**
			 * @brief Construct a new Circular Array object
			 *
			 * @param n size of the container
			 * @param value value to initialize each element with
			 */
			CircularArray(size_t n, const T& value);


			/**
			 * @brief Returns the number of elements that can be held in currently allocated storage
			 *
			 * @return Capacity of the currently allocated storage
			 */
			size_t capacity() const noexcept;

			/**
			 * @brief Returns the number of elements currently used in the container
			 *
			 * @return The number of elements in the container
			 */
			size_t size() const noexcept;


			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Reference to the requested element
			 */
			T& at(ptrdiff_t index);

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Const reference to the requested element
			 */
			const T& at(ptrdiff_t index) const;

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Reference to the requested element
			 */
			T& operator[](ptrdiff_t index);

			/**
			 * @brief Access specified element
			 *
			 * @param index position of the element to return
			 * @return Const reference to the requested element
			 */
			const T& operator[](ptrdiff_t index) const;


			/**
			 * @brief Returns the corresponding index in the underlying storage
			 *
			 * @param index position of the element
			 * @return Absolute index position of the element in the underlying storage
			 */
			size_t absoluteIndex(ptrdiff_t index) const noexcept;

			/**
			 * @brief Access specified element in underlying storage
			 *
			 * @param index absolute index position of the element to return
			 * @return Reference to the requested element
			 */
			T& atAbsoluteIndex(size_t index);

			/**
			 * @brief Access specified element in underlying storage
			 *
			 * @param index absolute index position of the element to return
			 * @return Const reference to the requested element
			 */
			const T& atAbsoluteIndex(size_t index) const;

			/**
			 * @brief Checks wether absolute index is in the currently used circular range
			 *
			 * @param index absolute index position to check
			 * @return *true* if absolute index is in bounds
			 * @return *false* otherwise
			 */
			bool absoluteIndexIsInBounds(size_t index) const noexcept;


			/**
			 * @brief Access the first element
			 *
			 * @return Reference to the first element
			 */
			T& front();

			/**
			 * @brief Access the first element
			 *
			 * @return Const reference to the first element
			 */
			const T& front() const;

			/**
			 * @brief Access the last element
			 *
			 * @return Reference to the last element
			 */
			T& back();

			/**
			 * @brief Access the last element
			 *
			 * @return Const reference to the last element
			 */
			const T& back() const;


			/**
			 * @brief Moves the beginning of the circular array
			 *
			 * @param indexes the number of indexes to add to the current beginning
			 */
			void moveBegin(ptrdiff_t indexes) noexcept;

			/**
			 * @brief Moves the end of the circular array
			 *
			 * @param indexes the number of indexes to add to the current end
			 */
			void moveEnd(ptrdiff_t indexes) noexcept;


			/**
			 * @brief Resets beginning and end indexes
			 */
			void reset() noexcept;

			/**
			 * @brief Resets beginning and end indexes and resizes the container
			 *
			 * @param capacity new size of the container
			 */
			void reset(size_t capacity) noexcept;

			/**
			 * @brief Resets beginning and end indexes and resizes the container
			 *
			 * @param capacity new size of the container
			 * @param value value to re-initialize all elements with
			 */
			void reset(size_t capacity, const T& value) noexcept;

			/**
			 * @brief Resets beginning and end indexes and the container's elements
			 *
			 * @param value value to re-initialize all elements with
			 */
			void reset(const T& value) noexcept;
	};
}

#include "CircularArray.cc"

#endif // CIRCULAR_ARRAY_H
