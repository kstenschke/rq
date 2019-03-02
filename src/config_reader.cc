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

#include <json-c/json_object.h>
#include <string>
#include <json-c/json.h>

#include "helper_string.h"
#include "helper_file.h"

void ReadConfig(std::string &path_config,
                const char *&url,
                const char *&user_agent,
                bool &use_ajax,
                bool &write_response_body_to_file,
                json_object *&config_obj) {
  std::__1::string config_json = helper::File::FileGetContents(path_config);
  config_obj = json_tokener_parse(config_json.c_str());

  struct json_object *url_obj;
  json_object_object_get_ex(config_obj, "url", &url_obj);
  url = json_object_get_string(url_obj);

  struct json_object *user_agent_obj;
  json_object_object_get_ex(config_obj, "user_agent", &user_agent_obj);
  user_agent = json_object_get_string(user_agent_obj);

  struct json_object *use_ajax_obj;
  json_object_object_get_ex(config_obj, "user_ajax", &use_ajax_obj);
  use_ajax = json_object_get_int(use_ajax_obj)==1;

  struct json_object *write_response_body_to_file_obj;
  json_object_object_get_ex(config_obj, "write_response_body_to_file", &write_response_body_to_file_obj);
  write_response_body_to_file = json_object_get_int(write_response_body_to_file_obj)==1;
}

void ReadCookiesConfig(const json_object *config_obj, const char *&cookie_domain, json_object *&cookie_items_obj) {
  struct json_object *cookie_obj;
  struct json_object *cookie_domain_obj;
  json_object_object_get_ex(const_cast<json_object *>(config_obj), "cookie", &cookie_obj);
  json_object_object_get_ex(cookie_obj, "domain", &cookie_domain_obj);
  cookie_domain = json_object_get_string(cookie_domain_obj);
  json_object_object_get_ex(cookie_obj, "values", &cookie_items_obj);
}

const char *ReadPostFieldsConfig(const json_object *config_obj) {
  const char *post_fields;
  struct json_object *post_fields_obj;
  json_object_object_get_ex(const_cast<json_object *>(config_obj), "post_fields", &post_fields_obj);
  post_fields = helper::String::StrReplaceAll(json_object_get_string(post_fields_obj), "\", \"", "&").c_str();
  return post_fields;
}
