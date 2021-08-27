#ifndef MY_PROJECT_PARSE_REQUEST_H
#define MY_PROJECT_PARSE_REQUEST_H

#include "request.h"
#include <string>

std::string PreprocessString(std::string str);
std::unique_ptr<Request> ParseBuildRequest(std::string& str);
std::unique_ptr<Request> ParseAnswRequest(std::string& str);

#endif  // MY_PROJECT_PARSE_REQUEST_H
