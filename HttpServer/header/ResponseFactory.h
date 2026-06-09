#pragma once
#ifndef RESPONSEFACTORY_H
#define RESPONSEFACTORY_H

#include "HttpTypes.h"
#include <string>

namespace ResponseFactory {

	Response dummy();

	Response badRequest(const std::string_view message);

	Response serverError();

	Response customText(const std::string msg);
}

#endif //RESPONSEFACTORY_H
