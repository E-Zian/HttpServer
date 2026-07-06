#pragma once
#ifndef HTTPSERVER_RATELIMITER_H
#define HTTPSERVER_RATELIMITER_H

#include <unordered_map>
#include <string>
#include <chrono>

namespace constants {
    inline constexpr std::chrono::seconds sweepInterval{ 60 };
    // Ip address idle for 5 mins
    inline constexpr std::chrono::seconds ipIdleThreshold{ 300 };
}

struct Bucket{
    double tokens{};
    std::chrono::steady_clock::time_point lastRequest;
};
class RateLimiter {
    public:
        RateLimiter(double bucketTokenCapacity,double tokenRefillPerSec);

        bool checkClientLimit(const std::string& clientIp);

    private:
        double bucketTokenCapacity_{} ;
        double tokenRefillPerSec_{} ;
        std::unordered_map<std::string,Bucket> clientBucket_{};

        void sweep(std::chrono::steady_clock::time_point& now);
};

#endif //HTTPSERVER_RATELIMITER_H