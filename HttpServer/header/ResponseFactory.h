#pragma once
#ifndef RESPONSEFACTORY_H
#define RESPONSEFACTORY_H

#include "HttpTypes.h"
#include <string>

namespace ResponseFactory {

	Response dummy();

	Response badRequest(std::string_view message);

	Response serverError();

}

#endif //RESPONSEFACTORY_H
