//
// MetricsSources.h
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

#include "SourceProcStat.h"
#include "SourceProcMeminfo.h"
#include "SourceProcNetDev.h"
#include "SourceEthtool.h"


namespace MetroCollect::MetricsSource {
	/**
	 * @brief Simple struct to hold references to each source singleton
	 */
	struct MetricsSourceArray {
		SourceProcStat& procStat;				//!< CPU metrics source (index: 1)
		SourceProcMeminfo& procMemInfo;			//!< Memory metrics source (index: 2)
		SourceProcNetDev& procNetDev;			//!< Network metrics source (index: 3)
		SourceEthtool& ethtool;					//!< Ethtool metrics source (index: 4)

		static constexpr size_t count = 4;		//!< Number of sources


		/**
		 * @brief Construct a new Metrics Source Array object
		 */
		MetricsSourceArray();


		/**
		 * @brief Execute a function for each source
		 *
		 * @tparam Function Function type, which must accept two arguments: implicitly convertible to the source type and an integer
		 * @param func function to execute
		 */
		template<typename Function>
		void forEach(Function func) {
			func(procStat, 0);
			func(procMemInfo, 1);
			func(procNetDev, 2);
			func(ethtool, 3);
		}

		/**
		 * @brief Execute a const function for each source
		 *
		 * @tparam Function Const function type, which must accept two arguments: implicitly convertible to the source type and an integer
		 * @param func function to execute
		 */
		template<typename Function>
		void forEach(Function func) const {
			func(procStat, 0);
			func(procMemInfo, 1);
			func(procNetDev, 2);
			func(ethtool, 3);
		}
	};
}
