
.. _program_listing_file_src_MetroCollect_SourceTools.cc:

Program Listing for File SourceTools.cc
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_MetroCollect_SourceTools.cc>` (``src/MetroCollect/SourceTools.cc``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: none

   //
   // SourceTools.cc
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
   
   #include "SourceTools.h"
   
   
   namespace MetroCollect::MetricsSource {
           bool resetFile(std::ifstream& file, std::vector<char>& buffer, const std::string_view& path) {
                   errno = 0;
                   buffer.clear();
                   buffer.resize(1024, '\0');
                   file = std::ifstream(path.data(), std::ios::binary);
                   if (errno) {
                           perror(std::string(path).append(": Error opening file").c_str());
                           errno = 0;
                           return false;
                   }
   
                   file.clear();
                   file.seekg(0);
                   file.read(buffer.data(), buffer.size());
                   if (errno) {
                           perror(std::string(path).append(": Error reading file").c_str());
                           errno = 0;
                           return false;
                   }
   
                   while (!file.eof()) {
                           buffer.resize(buffer.size() * 2, '\0');
                           file.clear();
                           file.seekg(0);
                           file.read(buffer.data(), buffer.size());
                   }
                   buffer.resize(buffer.size() * 2, '\0');
                   return true;
           }
   
           bool readFile(std::ifstream& file, std::vector<char>& buffer, const std::string_view& path) {
                   errno = 0;
                   file.clear();
                   file.seekg(0);
                   file.read(buffer.data(), buffer.size());
                   if (errno) {
                           perror(std::string(path).append(": Error reading file").c_str());
                           errno = 0;
                           return resetFile(file, buffer, path);
                   }
                   buffer[file.gcount()] = '\0';
                   return true;
           }
   
   
           int alphanumCompare(const char* l, const char* r) {
           enum mode_t { STRING, NUMBER } mode = STRING;
                   while (*l && *r) {
                           switch (mode) {
                                   case STRING:
                                           char l_char, r_char;
                                           while ((l_char = *l) && (r_char = *r)) {
                                                   const bool l_digit = std::isdigit(l_char);
                                                   const bool r_digit = std::isdigit(r_char);
                                                   if (l_digit && r_digit) {
                                                           mode = NUMBER;
                                                           break;
                                                   }
                                                   if (l_digit)
                                                           return -1;
                                                   if (r_digit)
                                                           return +1;
                                                   const int diff = l_char - r_char;
                                                   if(diff != 0)
                                                           return diff;
                                                   ++l;
                                                   ++r;
                                           }
                                           break;
                                   case NUMBER:
                                           char *end;
                                           unsigned long l_int = std::strtoul(l, &end, 0);
                                           l = end;
                                           unsigned long r_int = std::strtoul(r, &end, 0);
                                           r = end;
                                           const long diff = l_int - r_int;
                                           if(diff != 0)
                                                   return diff;
                                           mode = STRING;
                                           break;
                           }
                   }
                   if (*r)
                           return -1;
                   if (*l)
                           return +1;
                   return 0;
           }
   }
