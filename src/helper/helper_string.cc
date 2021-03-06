/*
  Copyright (c) 2019, Kay Stenschke
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of  nor the names of its contributors may be used to
     endorse or promote products derived from this software without specific
     prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include <string>
#include <cstring>
#include <iostream>

#include "helper_string.h"

namespace helper {

std::string String::StrReplaceAll(std::string &haystack, const char *needle, const char *replacement) {
  size_t needle_len = std::strlen(needle);

  size_t index = 0;
  while (true) {
    index = haystack.find(needle, index);
    if (std::string::npos == index) break;

    haystack.replace(index, needle_len, replacement);
  }
  // TODO ensure avoid endless looping (when e.g. replacing "a" by "aa")
  return haystack;
}

std::string String::StrReplaceAll(const char *haystack, const char *needle, const char *replacement) {
  std::string str = std::string(haystack);
  return StrReplaceAll(str, needle, replacement);
}

bool String::StrContains(char *haystack, const char *needle) {
  return std::string::npos != std::string(haystack, strlen(haystack)).find(needle);
}

int String::GetAmountDigits(int number) {
  std::string str = std::to_string(number);

  return static_cast<int>(str.length());
}

std::string String::UrlToFilename(const char *url, int ordinal_prefix, int amount_prefix_digits) {
  std::string filename;

  filename = StrReplaceAll(url, "https://", "");
  filename = StrReplaceAll(filename, "http://", "");
  filename = StrReplaceAll(filename, "/", "-");

  std::string prefix = std::to_string(ordinal_prefix);
  while (prefix.length() < amount_prefix_digits) {
    std::string zero = "0";
    prefix = zero.append(prefix);
  }

  return prefix.append("-").append(filename);
}

} // namespace helper
