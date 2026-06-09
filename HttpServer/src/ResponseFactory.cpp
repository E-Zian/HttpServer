#include "ResponseFactory.h"

Response ResponseFactory::badRequest(const std::string_view message) {
	Response response{HttpStatus::BAD_REQUEST,{},std::string{message}};
	response.header["Content-Type"] = "text/plain";
	response.header["Content-Length"] = std::to_string(message.length()) ;

	return response;
}

Response ResponseFactory::serverError() {
	const std::string msg{ "Internal Server Error Occurred " };

	Response response{HttpStatus::SERVER_ERROR,{},msg};
	response.header["Content-Type"] = "text/plain";
	response.header["Content-Length"] = std::to_string(msg.length()) ;

	return response;
}

Response ResponseFactory::dummy() {
	const std::string msg{ "This is a dummy text response" };
	Response response(HttpStatus::OK, {}, msg);
	response.header["Content-Type"] = "text/plain";
	response.header["Content-Length"] = std::to_string(msg.length()) ;

	return response;
}

Response ResponseFactory::customText(const std::string msg) {
	Response response(HttpStatus::OK, {}, msg);
	response.header["Content-Type"] = "text/plain";
	response.header["Content-Length"] = std::to_string(msg.length());

	return response;
}
