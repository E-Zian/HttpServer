#ifndef HTTPSERVER_HOMECONTROLLER_H
#define HTTPSERVER_HOMECONTROLLER_H
#include "controller/ControllerBase.h"

class HomeController :public ControllerBase{
    public:
    explicit HomeController(Router& router);
    private:
};


#endif //HTTPSERVER_HOMECONTROLLER_H