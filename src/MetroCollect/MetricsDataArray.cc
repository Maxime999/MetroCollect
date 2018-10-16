//
// MetricsDataArray.cc
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

#include "MetricsDataArray.h"


namespace MetroCollect {
	MetricsDataArray::MetricsDataArray() noexcept : MetricsArray() { }

	MetricsDataArray::MetricsDataArray(MetricsSource::SourceInterests sourceInterests) noexcept : MetricsArray(sourceInterests) { }


	void MetricsDataArray::updateData() {
		this->sources_.forEach([&](auto& source, size_t sourceIndex) {
			if ((*this->sourceInterests_)[sourceIndex].any())
				source.fetchData((*this->sourceInterests_)[sourceIndex], this->sourceIterators_[sourceIndex].first);
		});

		this->timestamp_ = std::chrono::system_clock::now();
	}
}
