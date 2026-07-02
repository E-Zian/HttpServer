#pragma once
#ifndef HTTP_REQUEST_PARSER
#define HTTP_REQUEST_PARSER

#include "model/HttpTypes.h"
#include <unordered_map>
#include <string>

struct ParseResultObject {
	ParsedRequestObject parseRequestObject;
	bool result;
	HttpStatus httpStatus;
	std::string errorMessage;
};

class HttpRequestParser {
public:
	static ParseResultObject parseHeader(std::string_view rawHeader);
private:

};

#endif 
