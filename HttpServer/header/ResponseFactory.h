#pragma once
#ifndef RESPONSEFACTORY_H
#define RESPONSEFACTORY_H

#include "model/HttpTypes.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <string>

namespace ResponseFactory {

	Response dummy();

	template<typename...Args>
	Response serverError(fmt::format_string<Args...> fmt_string, Args&&... args) {
		std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		Response response{ HttpStatus::SERVER_ERROR,{},message };
		response.header["Content-Type"] = "text/plain";
		response.header["Content-Length"] = std::to_string(message.length());

		return response;
	}

	Response customText(const std::string msg);

	template<typename...Args>
	Response badRequest(fmt::format_string<Args...> fmt_string,Args&&... args) {
		std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		Response response{ HttpStatus::BAD_REQUEST,{},std::string{message} };
		response.header["Content-Type"] = "text/plain";
		response.header["Content-Length"] = std::to_string(message.length());

		return response;
	}

	Response dummyJson();


}

#endif //RESPONSEFACTORY_H
