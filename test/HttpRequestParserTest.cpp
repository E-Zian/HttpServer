#include "HttpRequestParser.h"
#include "model/HttpTypes.h"
#include <gtest/gtest.h>

// The parser receives the header block WITHOUT the trailing "\r\n\r\n"
// (Connection strips those 4 bytes before calling parseHeader), with
// individual header lines separated by "\r\n".

TEST(HttpRequestParserTest, Parses_Request_Line_Only) {
    const ParseResultObject result{HttpRequestParser::parseHeader("GET /api/user HTTP/1.1")};

    EXPECT_TRUE(result.result) << "A well-formed request line should parse";
    EXPECT_EQ(result.parseRequestObject.method, Method::GET);
    EXPECT_EQ(result.parseRequestObject.route, "/api/user");
    EXPECT_EQ(result.parseRequestObject.httpVersion, "HTTP/1.1");
    EXPECT_TRUE(result.parseRequestObject.header.empty()) << "No header lines were sent";
}

TEST(HttpRequestParserTest, Parses_Headers) {
    const ParseResultObject result{
        HttpRequestParser::parseHeader("POST /api/user HTTP/1.1\r\nHost: localhost\r\nContent-Length: 15")
    };

    ASSERT_TRUE(result.result);
    EXPECT_EQ(result.parseRequestObject.method, Method::POST);
    ASSERT_TRUE(result.parseRequestObject.header.contains("host"));
    EXPECT_EQ(result.parseRequestObject.header.at("host"), "localhost");
    ASSERT_TRUE(result.parseRequestObject.header.contains("content-length"));
    EXPECT_EQ(result.parseRequestObject.header.at("content-length"), "15");
}

TEST(HttpRequestParserTest, Header_Keys_Are_Lowercased_And_Values_Trimmed) {
    const ParseResultObject result{
        HttpRequestParser::parseHeader("GET / HTTP/1.1\r\nContent-Type:   application/json   ")
    };

    ASSERT_TRUE(result.result);
    // key normalised to lower case, value stripped of surrounding whitespace
    ASSERT_TRUE(result.parseRequestObject.header.contains("content-type"));
    EXPECT_EQ(result.parseRequestObject.header.at("content-type"), "application/json");
}

TEST(HttpRequestParserTest, Header_Splits_On_First_Colon_Only) {
    const ParseResultObject result{
        HttpRequestParser::parseHeader("GET / HTTP/1.1\r\nHost: localhost:8080")
    };

    ASSERT_TRUE(result.result);
    ASSERT_TRUE(result.parseRequestObject.header.contains("host"));
    EXPECT_EQ(result.parseRequestObject.header.at("host"), "localhost:8080")
        << "Only the first colon separates key from value";
}

TEST(HttpRequestParserTest, Maps_All_Supported_Methods) {
    EXPECT_EQ(HttpRequestParser::parseHeader("GET / HTTP/1.1").parseRequestObject.method, Method::GET);
    EXPECT_EQ(HttpRequestParser::parseHeader("POST / HTTP/1.1").parseRequestObject.method, Method::POST);
    EXPECT_EQ(HttpRequestParser::parseHeader("PUT / HTTP/1.1").parseRequestObject.method, Method::PUT);
    EXPECT_EQ(HttpRequestParser::parseHeader("DELETE / HTTP/1.1").parseRequestObject.method, Method::DEL);
    EXPECT_EQ(HttpRequestParser::parseHeader("PATCH / HTTP/1.1").parseRequestObject.method, Method::PATCH);
    EXPECT_EQ(HttpRequestParser::parseHeader("HEAD / HTTP/1.1").parseRequestObject.method, Method::HEAD);
    EXPECT_EQ(HttpRequestParser::parseHeader("OPTIONS / HTTP/1.1").parseRequestObject.method, Method::OPTIONS);
}

TEST(HttpRequestParserTest, Rejects_Malformed_Request_Line) {
    // Missing the HTTP version -> fewer than 3 space-separated tokens
    const ParseResultObject result{HttpRequestParser::parseHeader("GET /api/user")};

    EXPECT_FALSE(result.result);
    EXPECT_EQ(result.httpStatus, HttpStatus::BAD_REQUEST);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST(HttpRequestParserTest, Rejects_Unsupported_Method) {
    const ParseResultObject result{HttpRequestParser::parseHeader("TRACE / HTTP/1.1")};

    EXPECT_FALSE(result.result);
    EXPECT_EQ(result.httpStatus, HttpStatus::BAD_REQUEST);
}

TEST(HttpRequestParserTest, Rejects_Malformed_Header_Line) {
    // Second line has no colon separating key and value
    const ParseResultObject result{HttpRequestParser::parseHeader("GET / HTTP/1.1\r\nNoColonHere")};

    EXPECT_FALSE(result.result);
    EXPECT_EQ(result.httpStatus, HttpStatus::BAD_REQUEST);
}
