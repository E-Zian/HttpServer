#include "Router.h"
#include "model/HttpTypes.h"
#include <gtest/gtest.h>
#include <stdexcept>

namespace {
    // A handler that echoes a marker into the response body so tests can prove
    // the correct route was invoked.
    std::function<Response(ParsedRequestObject&)> markerHandler(const std::string& marker) {
        return [marker](ParsedRequestObject&) {
            Response response{HttpStatus::OK, {}, marker};
            return response;
        };
    }
}

TEST(RouterTest, Dispatches_Static_Route) {
    Router router;
    router.addRoute(Method::GET, "/api/test", markerHandler("hit"));

    ParsedRequestObject request{};
    request.method = Method::GET;
    request.route = "/api/test";

    const Response response{router.dispatch(request)};

    EXPECT_EQ(response.status, HttpStatus::OK);
    EXPECT_EQ(response.body, "hit");
}

TEST(RouterTest, Extracts_Path_Parameter) {
    Router router;
    router.addRoute(Method::GET, "/api/user/:id", markerHandler("user"));

    ParsedRequestObject request{};
    request.method = Method::GET;
    request.route = "/api/user/42";

    const Response response{router.dispatch(request)};

    EXPECT_EQ(response.body, "user");
    ASSERT_TRUE(request.parameterValues.contains(":id"));
    EXPECT_EQ(request.parameterValues.at(":id"), "42") << "The :id segment should be captured";
}

TEST(RouterTest, Unknown_Route_Returns_Not_Found) {
    Router router;
    router.addRoute(Method::GET, "/api/test", markerHandler("hit"));

    ParsedRequestObject request{};
    request.method = Method::GET;
    request.route = "/does/not/exist";

    EXPECT_EQ(router.dispatch(request).status, HttpStatus::NOT_FOUND);
}

TEST(RouterTest, Wrong_Method_On_Existing_Route_Returns_Not_Found) {
    Router router;
    router.addRoute(Method::GET, "/api/test", markerHandler("hit"));

    ParsedRequestObject request{};
    request.method = Method::POST; // route exists, but only for GET
    request.route = "/api/test";

    // NOTE: the server currently returns 404 here; a stricter HTTP
    // implementation would return 405 Method Not Allowed.
    EXPECT_EQ(router.dispatch(request).status, HttpStatus::NOT_FOUND);
}

TEST(RouterTest, Conflicting_Parameter_Names_Throw) {
    Router router;
    router.addRoute(Method::GET, "/api/:id", markerHandler("a"));

    // A second parameter segment with a different name at the same position
    // is ambiguous and must be rejected at registration time.
    EXPECT_THROW(
        router.addRoute(Method::GET, "/api/:name", markerHandler("b")),
        std::invalid_argument);
}

TEST(RouterTest, Same_Path_Supports_Multiple_Methods) {
    Router router;
    router.addRoute(Method::GET, "/api/user", markerHandler("get"));
    router.addRoute(Method::POST, "/api/user", markerHandler("post"));

    ParsedRequestObject getRequest{};
    getRequest.method = Method::GET;
    getRequest.route = "/api/user";
    EXPECT_EQ(router.dispatch(getRequest).body, "get");

    ParsedRequestObject postRequest{};
    postRequest.method = Method::POST;
    postRequest.route = "/api/user";
    EXPECT_EQ(router.dispatch(postRequest).body, "post");
}
