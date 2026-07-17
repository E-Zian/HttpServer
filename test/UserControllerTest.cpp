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

	nlohmann::json json{nlohmann::json::parse(response.body)};

	EXPECT_EQ(json["user"].get<UserModel::User>(), fakeUser)<< "Incorrect user";
	EXPECT_EQ(json["message"], "User created successfully") << "Incorrect Message";
}