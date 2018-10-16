#!/bin/bash
#
# mergeheaders.sh
# Script to merge CircularArray's .h and .cc files
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

SOURCE_PATH="$1"
FILE="$2"

cat $SOURCE_PATH/CircularArray.h | grep -v "CircularArray.cc" | grep -v 'endif' > $FILE
echo "#define CIRCULAR_ARRAY_CC" >> $FILE
echo >> $FILE
echo >> $FILE
cat $SOURCE_PATH/CircularArray.cc | awk 'BEGIN{ found=0 } /namespace/{ found=1 }  { if (found) print }' >> $FILE
echo >> $FILE
