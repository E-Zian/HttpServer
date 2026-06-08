#include "ResponseFactory.h"

//Response ResponseFactory::badRequest(std::string_view message) {
//
//}
//
//Response ResponseFactory::serverError(std::string_view message) {
//
//}

Response ResponseFactory::dummy() {
	std::string msg{ "This is a dummy text response" };
	Response response(HttpStatus::OK, {}, msg);
	response.header["Content-Type"] = "text/plain";
	response.header["Content-Length"] = msg.length();

	return response;
}
