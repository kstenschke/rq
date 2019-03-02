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
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include "helper_file.h"
#include "helper_string.h"

namespace helper {

/**
 * Get absolute path to application executable
 */
std::string File::GetBinaryPath(char **argv, size_t strLenExecutableName) {
  char *path_relative = argv[0];
  char absolute_path[255];
  char *ptr;
  ptr = realpath(path_relative, absolute_path);

  unsigned long len_without_binary = strlen(absolute_path) - strLenExecutableName;

  return std::string(ptr).substr(0, len_without_binary);
}

bool File::FileExists(const std::string &name) {
  return access(name.c_str(), F_OK) != -1;
}

std::string File::FileStreamGetContents(std::ifstream &file) {
  // Get filesize
  file.seekg(0, std::ios::end);
  std::streampos length = file.tellg();
  file.seekg(0, std::ios::beg);

  // Read the whole file into the buffer
  std::vector<char> buffer(static_cast<unsigned long>(length));
  file.read(&buffer[0], length);

  std::string str(buffer.begin(), buffer.end());

  return str;
}

void File::AddFileExtensionByContentType(std::string &path_binary,
                                   std::string &filename_response_body,
                                   char *content_type) {
  if (content_type) {
    std::string filename_old = path_binary.append(filename_response_body);
    std::string filename_new;
    if (helper::String::StrContains(content_type, "html")) {
      filename_new = path_binary.append(filename_response_body.append(".html"));
    } else if (helper::String::StrContains(content_type, "xml")) {
      filename_new = path_binary.append(filename_response_body.append(".xml"));
    } else {
      filename_new = path_binary.append(filename_response_body.append(".json"));
    }
    rename(filename_old.c_str(), filename_new.c_str());
  }
}

std::string File::FileGetContents(std::string &filename) {
  std::ifstream file(filename);

  return FileStreamGetContents(file);
}

} // namespace helper
