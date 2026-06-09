#include "ResponseFactory.h"

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
