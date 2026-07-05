#pragma once
#ifndef HTTPSERVER_RATELIMITER_H
#define HTTPSERVER_RATELIMITER_H

#include <unordered_map>
#include <string>
#include <chrono>

struct Bucket{
    double tokens{};
    std::chrono::steady_clock::time_point lastRequest;
};
class RateLimiter {
    public:
        RateLimiter(double bucketTokenCapacity,double tokenRefillPerMin);
        std::optional<bool> checkClientLimit(const std::string& clientIp);

    private:
        double bucketTokenCapacity_{} ;
        double tokenRefillPerMin_{} ;
        std::unordered_map<std::string,Bucket> clientBucket{};
};

#endif //HTTPSERVER_RATELIMITER_H