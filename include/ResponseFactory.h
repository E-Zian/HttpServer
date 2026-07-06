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
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ HttpStatus::SERVER_ERROR,{},json.dump() };
		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		return response;
	}

	Response customText(const std::string &msg);

	template<typename...Args>
	Response badRequest(fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ HttpStatus::BAD_REQUEST,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());


		return response;
	}

	Response dummyJson();

	template<typename...Args>
	Response notFound(fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ HttpStatus::NOT_FOUND,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		return response;
	}

	template<typename...Args>
	Response requestTimeout(fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ HttpStatus::REQUEST_TIMEOUT,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		return response;
	}

	template<typename...Args>
	Response headerTooLarge(fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ HttpStatus::REQUEST_HEADERS_TOO_LARGE,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		return response;
	}

	template<typename...Args>
	Response contentTooLarge(fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ HttpStatus::CONTENT_TOO_LARGE,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		return response;
	}

	template<typename...Args>
	Response failedResponse(HttpStatus status,fmt::format_string<Args...> fmt_string,Args&&... args) {
		const std::string message{ fmt::format(fmt_string,std::forward<Args>(args)...) };

		nlohmann::json json;
		json["errorMessage"] = message;

		Response response{ status,{},json.dump() };

		response.header["content-type"] = "application/json";
		response.header["connection"] = "close";
		response.header["content-length"] = std::to_string(response.body.size());

		return response;
	}


}

#endif //RESPONSEFACTORY_H
