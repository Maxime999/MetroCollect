#
# CMakeList.txt
# Circular Array library cmake file
#
# Copyright 2018 CFM (www.cfm.fr)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


FILE(GLOB CircularArraySources *.cc)
FILE(GLOB CircularArrayHeaders *.h)

add_library(CircularArray STATIC ${CircularArraySources})

add_custom_target(CircularArrayHeader ALL
	# DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/CircularArray.h
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mergeheaders.sh "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CircularArray.h")

# add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CircularArray.h
# 	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mergeheaders.sh "${CMAKE_CURRENT_BINARY_DIR}/CircularArray.h"
# 	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CircularArray.h
# 	DESTINATION include/CircularArray)
