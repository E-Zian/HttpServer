#include "RateLimiter.h"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

TEST(RateLimiterTest, First_Request_From_New_Ip_Is_Allowed) {
    RateLimiter limiter{5.0, 1.0};

    const CheckLimitResult result{limiter.checkClientLimit("1.2.3.4")};

    EXPECT_TRUE(result.allow);
    EXPECT_EQ(result.tokenCapacity, 5.0);
    EXPECT_EQ(result.tokensLeft, 4.0) << "A new bucket starts at capacity - 1";
}

TEST(RateLimiterTest, Empty_Ip_Is_Rejected) {
    RateLimiter limiter{5.0, 1.0};

    const CheckLimitResult result{limiter.checkClientLimit("")};

    EXPECT_FALSE(result.allow);
}

TEST(RateLimiterTest, Burst_Is_Capped_At_Capacity) {
    // No refill, so exactly `capacity` requests get through before throttling.
    RateLimiter limiter{3.0, 0.0};

    int allowed{};
    for (int i{}; i < 10; ++i) {
        if (limiter.checkClientLimit("1.2.3.4").allow) {
            ++allowed;
        }
    }

    EXPECT_EQ(allowed, 3) << "Capacity of 3 with no refill allows 3 requests";
}

TEST(RateLimiterTest, Buckets_Are_Tracked_Per_Ip) {
    RateLimiter limiter{2.0, 0.0};

    // Exhaust the first client entirely.
    limiter.checkClientLimit("1.1.1.1");
    limiter.checkClientLimit("1.1.1.1");
    EXPECT_FALSE(limiter.checkClientLimit("1.1.1.1").allow) << "First client is now throttled";

    // A different IP has its own independent bucket.
    EXPECT_TRUE(limiter.checkClientLimit("2.2.2.2").allow) << "Second client is unaffected";
}

TEST(RateLimiterTest, Tokens_Refill_Over_Time) {
    // capacity 2, refill 100 tokens/sec (1 token every 10ms). The low rate keeps
    // the "drained" state stable against scheduling jitter between calls, while a
    // 50ms wait comfortably refills at least one token.
    RateLimiter limiter{2.0, 100.0};

    // Drain the bucket: new IP starts with 1 token, second call consumes it.
    limiter.checkClientLimit("1.2.3.4"); // allowed, bucket now ~0
    limiter.checkClientLimit("1.2.3.4"); // consumes remaining token
    EXPECT_FALSE(limiter.checkClientLimit("1.2.3.4").allow) << "Bucket should be empty";

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(limiter.checkClientLimit("1.2.3.4").allow) << "Bucket should have refilled";
}
