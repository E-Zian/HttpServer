#pragma once
#ifndef RESPONSEFACTORY_H
#define RESPONSEFACTORY_H

#include "Helper.h"
#include "model/HttpTypes.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <string>

namespace ResponseFactory {
	Response dummy();

	Response customText(const std::string &msg);

	Response dummyJson();

	template<typename...Args>
	Response failedResponse(const HttpStatus status,fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;
		
		Response response{ status,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		//Helper::displayError(message);
		return response;
	}


}

#endif //RESPONSEFACTORY_H
