#include "Interface/IUserRepo.h"
#include "model/UserModel.h"
#include "Router.h"
#include "model/HttpTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "controller/UserController.h"

class MockUserRepo : public IUserRepo {
	public:
	using User = UserModel::User;
    MOCK_METHOD((std::optional<User>),createUser,(const UserModel::DTO::CreateUserRequest &createUserRequest),(const,override));

	MOCK_METHOD((std::optional<std::vector<User>>),getAllUser,(),(const,override));

	MOCK_METHOD(std::optional<UserModel::User>, getUserById,
			(int), (const, override));

	MOCK_METHOD(std::optional<UserModel::User>, updateUser,
				(const UserModel::User&), (const, override));

	MOCK_METHOD(bool, deleteUserById,
				(int), (const, override));
};

// Create User

TEST(UserControllerTest, Create_User_Success) {
	const MockUserRepo repo;

	const UserModel::User fakeUser{1,"testing_user","test@gmail.com"};
	EXPECT_CALL(repo,createUser(testing::_)).WillOnce(testing::Return(fakeUser));

	const Router router;
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/user";
	request.body = R"({"user": {"name": "testing_user","email":"est@gmail.com"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Http Status not equal to OK";

	nlohmann::json json = nlohmann::json::parse(response.body);

	EXPECT_EQ(json["user"].get<UserModel::User>(), fakeUser)<< "Incorrect user";
	EXPECT_EQ(json["message"], "User created successfully") << "Incorrect Message";
}

TEST(UserControllerTest,User_Already_Exist) {
	const MockUserRepo repo;
	EXPECT_CALL(repo,createUser(testing::_)).WillOnce(testing::Return(std::nullopt));

	const Router router;

	UserController userController(router,repo);
	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/user";
	request.body = R"({"user": {"name": "testing_user","email":"est@gmail.com"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Http Status not equal to Bad request";

	nlohmann::json json = nlohmann::json::parse(response.body);

	EXPECT_EQ(json["errorMessage"],"User email already exists") << "Incorrect Error Message";
}

TEST(UserControllerTest,Incorrect_Json_Body) {
	const MockUserRepo repo;
	const Router router;

	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/user";
	request.body = R"({"Not_user": { "name":"test","email":"test@gmail.com" }})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Http Status not equal to Bad request";
	nlohmann::json json = nlohmann::json::parse(response.body);

	EXPECT_EQ(json["errorMessage"],"No user received") << "No user received";
}

TEST(UserControllerTest,Malformed_JSON) {
	const MockUserRepo repo;
	const Router router;

	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/user";
	request.body = R"({"Not_u"name":"test","email"sail.com" }}}}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Http Status not equal to Bad request";
	nlohmann::json json = nlohmann::json::parse(response.body);

	EXPECT_EQ(json["errorMessage"],"Invalid JSON body") << "No user received";
}

// Get All Users

TEST(UserControllerTest,Get_All_User) {
	const MockUserRepo repo;
	const Router router;

	std::vector<UserModel::User> fakeUsers{{1,"test1","test1@gmail.com"},{2,"test2","test2@gmail.com"}};

	EXPECT_CALL(repo,getAllUser).WillOnce(testing::Return(fakeUsers));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user";

	Response response {router.dispatch(request)};

	nlohmann::json json = nlohmann::json::parse(response.body);

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	std::string expectedJsonString {R"([{"id":1,"name":"test1","email":"test1@gmail.com"},{"id":2,"name":"test2","email":"test2@gmail.com"}])"};
	nlohmann::json expectedJson = nlohmann::json::parse(expectedJsonString);
	EXPECT_EQ(json["users"],expectedJson);
}

TEST(UserControllerTest,Get_All_User_Server_Error) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,getAllUser).WillOnce(testing::Return(std::nullopt));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::SERVER_ERROR) << "Status not server error";
}

TEST(UserControllerTest,Get_All_User_Empty) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,getAllUser).WillOnce(testing::Return(std::vector<UserModel::User>{}));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["users"].size(), 0u);
	EXPECT_EQ(json["message"], "Total 0 users retrieved");
}

// Get User

TEST(UserControllerTest,Get_User_Success) {
	const MockUserRepo repo;
	const Router router;

	const UserModel::User fakeUser{42,"carol","carol@gmail.com"};
	EXPECT_CALL(repo,getUserById(42)).WillOnce(testing::Return(fakeUser));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user/42";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["user"].get<UserModel::User>(), fakeUser) << "Incorrect user";
	EXPECT_EQ(json["message"], "User retrieved successfully");
}

TEST(UserControllerTest,Get_User_Not_Found) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,getUserById(999)).WillOnce(testing::Return(std::nullopt));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user/999";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::NOT_FOUND) << "Status not 404";
}

TEST(UserControllerTest,Get_User_Invalid_Id) {
	const MockUserRepo repo;
	const Router router;

	// stoi("abc") throws -> controller returns 400 before touching the repo.
	EXPECT_CALL(repo,getUserById(testing::_)).Times(0);
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user/abc";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

TEST(UserControllerTest,Get_User_Id_Out_Of_Range) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,getUserById(testing::_)).Times(0);
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/user/99999999999999999999";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

// Update User

TEST(UserControllerTest,Update_User_Success) {
	const MockUserRepo repo;
	const Router router;

	const UserModel::User updated{5,"new_name","new@gmail.com"};
	EXPECT_CALL(repo,updateUser(testing::_)).WillOnce(testing::Return(updated));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/user/5";
	request.body = R"({"user": {"name": "new_name","email":"new@gmail.com"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["user"]["id"], 5);
	EXPECT_EQ(json["message"], "User updated successfully");
}

TEST(UserControllerTest,Update_User_No_User_Field) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,updateUser(testing::_)).Times(0);
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/user/5";
	request.body = R"({"nope": true})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

TEST(UserControllerTest,Update_User_Not_Found) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,updateUser(testing::_)).WillOnce(testing::Return(std::nullopt));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/user/5";
	request.body = R"({"user": {"name": "x","email":"x@gmail.com"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::NOT_FOUND) << "Status not 404";
}

TEST(UserControllerTest,Update_User_Invalid_Id) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,updateUser(testing::_)).Times(0);
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/user/abc";
	request.body = R"({"user": {"name": "x","email":"x@gmail.com"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

// Delete User

TEST(UserControllerTest,Delete_User_Success) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,deleteUserById(7)).WillOnce(testing::Return(true));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::DEL;
	request.route = "/api/user/7";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["message"], "User id 7 deleted successfully");
}

TEST(UserControllerTest,Delete_User_Not_Found) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,deleteUserById(7)).WillOnce(testing::Return(false));
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::DEL;
	request.route = "/api/user/7";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::NOT_FOUND) << "Status not 404";
}

TEST(UserControllerTest,Delete_User_Invalid_Id) {
	const MockUserRepo repo;
	const Router router;

	EXPECT_CALL(repo,deleteUserById(testing::_)).Times(0);
	UserController userController(router,repo);

	ParsedRequestObject request{};
	request.method = Method::DEL;
	request.route = "/api/user/abc";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}
