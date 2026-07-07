#include "RateLimiter.h"
#include "Helper.h"
#include <chrono>

RateLimiter::RateLimiter(const double bucketTokenCapacity, const double tokenRefillPerSec) : bucketTokenCapacity_{
		bucketTokenCapacity
}, tokenRefillPerSec_(tokenRefillPerSec),
lastSweep_{ std::chrono::steady_clock::now() }
{
}

bool RateLimiter::checkClientLimit(const std::string& clientIp) {
	if (clientIp.empty()) {
		return false;
	}

	std::chrono::steady_clock::time_point timeNow{ std::chrono::steady_clock::now() };

	if (timeNow - lastSweep_> Constants::sweepInterval) {
		sweep(timeNow);
		lastSweep_ = timeNow;
	}

	auto clientIt{ clientBucket_.find(clientIp) };
	if (clientIt == clientBucket_.end()) {
		clientBucket_[clientIp] = Bucket{ bucketTokenCapacity_ - 1,timeNow };
		return true;
	}

	Bucket& clientBucket{ clientIt->second };

	std::chrono::duration<double> elapsedTime{ timeNow - clientBucket.lastRequest };
	clientBucket.lastRequest = timeNow;

	clientBucket.tokens += elapsedTime.count() * tokenRefillPerSec_;
	clientBucket.tokens = std::min(clientBucket.tokens, bucketTokenCapacity_);

	if (clientBucket.tokens < 1.0) {
		return false;
	}

	clientBucket.tokens -= 1.0;
	return true;
}

void RateLimiter::sweep(const std::chrono::steady_clock::time_point& now) {
	const auto removed = std::erase_if(clientBucket_, [&](const auto& entry) {
		return (now - entry.second.lastRequest) > Constants::ipIdleThreshold;
		});
	Helper::displayMessage("Rate limiter swept {} stale buckets", removed);
}