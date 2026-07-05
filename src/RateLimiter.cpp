#include "RateLimiter.h"

RateLimiter::RateLimiter(const double bucketTokenCapacity, const double tokenRefillPerMin) : bucketTokenCapacity_{
        bucketTokenCapacity
    }, tokenRefillPerMin_(tokenRefillPerMin)
{
}

bool RateLimiter::checkClientLimit(const std::string &clientIp) {
    if (clientIp.empty()) {
        return false;
    }

}
