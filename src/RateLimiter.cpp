#include "RateLimiter.h"
#include <chrono>

RateLimiter::RateLimiter(const double bucketTokenCapacity, const double tokenRefillPerSec) : bucketTokenCapacity_{
        bucketTokenCapacity
    }, tokenRefillPerSec_(tokenRefillPerSec)
{
}

bool RateLimiter::checkClientLimit(const std::string &clientIp) {
    if (clientIp.empty()) {
        return false;
    }

    auto clientIt{ clientBucket_.find(clientIp) };
    if (clientIt == clientBucket_.end()) {
        clientBucket_[clientIp]=Bucket{bucketTokenCapacity_-1,std::chrono::steady_clock::now() };
        return true;
    }

    Bucket& clientBucket{ clientIt->second };

    std::chrono::duration<double> elapsedTime{ std::chrono::steady_clock::now() - clientBucket.lastRequest };
    clientBucket.lastRequest = std::chrono::steady_clock::now();

    clientBucket.tokens += elapsedTime.count() * tokenRefillPerSec_;
    clientBucket.tokens = std::min(clientBucket.tokens, bucketTokenCapacity_);

    if (clientBucket.tokens < 1.0) {
        return false;
    }

    clientBucket.tokens -= 1.0;
    return true;
}

void RateLimiter::sweep(std::chrono::steady_clock::time_point& now) {
    std::erase_if
}