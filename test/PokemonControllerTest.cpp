#include "Interface/IPokemonRepo.h"
#include "model/PokemonModel.h"
#include "Router.h"
#include "model/HttpTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "controller/PokemonController.h"

class MockPokemonRepo : public IPokemonRepo {
public:
	MOCK_METHOD((std::optional<PokemonModel::Pokemon>),createPokemon,(const PokemonModel::DTO::CreatePokemonRequest&),(const,override));

	MOCK_METHOD((std::optional<PokemonModel::Pokemon>),getPokemonById,(int),(const,override));

	MOCK_METHOD((std::optional<PokemonModel::Pokemon>),updatePokemon,(const PokemonModel::Pokemon&),(const,override));

	MOCK_METHOD(bool,deletePokemonById,(int),(const,override));

	MOCK_METHOD((std::optional<std::vector<PokemonModel::Pokemon>>),getAllPokemon,(),(const,override));
};

// Create Pokemon

TEST(PokemonControllerTest,Create_Pokemon_Success) {
	const MockPokemonRepo repo;
	const Router router;

	const PokemonModel::Pokemon fakePokemon{1,"pikachu"};
	EXPECT_CALL(repo,createPokemon(testing::_)).WillOnce(testing::Return(fakePokemon));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/pokemon";
	request.body = R"({"pokemon": {"name": "pikachu"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["pokemon"].get<PokemonModel::Pokemon>(), fakePokemon) << "Incorrect pokemon";
	EXPECT_EQ(json["message"], "Pokemon created successfully");
}

TEST(PokemonControllerTest,Create_Pokemon_No_Pokemon_Field) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,createPokemon(testing::_)).Times(0);
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/pokemon";
	request.body = R"({"not_pokemon": {"name": "pikachu"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["errorMessage"], "No pokemon was passed");
}

TEST(PokemonControllerTest,Create_Pokemon_Already_Exists) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,createPokemon(testing::_)).WillOnce(testing::Return(std::nullopt));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/pokemon";
	request.body = R"({"pokemon": {"name": "pikachu"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["errorMessage"], "Pokemon already exists");
}

TEST(PokemonControllerTest,Create_Pokemon_Malformed_JSON) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,createPokemon(testing::_)).Times(0);
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::POST;
	request.route = "/api/pokemon";
	request.body = R"({"pokemon": {"name" broken json }}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["errorMessage"], "Invalid JSON");
}

// Get All Pokemon

TEST(PokemonControllerTest,Get_All_Pokemon_Success) {
	const MockPokemonRepo repo;
	const Router router;

	std::vector<PokemonModel::Pokemon> fakePokemons{{1,"pikachu"},{2,"bulbasaur"}};
	EXPECT_CALL(repo,getAllPokemon).WillOnce(testing::Return(fakePokemons));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/pokemon";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["pokemons"].size(), 2u);
	EXPECT_EQ(json["pokemons"][0]["name"], "pikachu");
	EXPECT_EQ(json["message"], "Total Pokemons Retrieved 2");
}

TEST(PokemonControllerTest,Get_All_Pokemon_Server_Error) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,getAllPokemon).WillOnce(testing::Return(std::nullopt));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/pokemon";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::SERVER_ERROR) << "Status not 500";
}

TEST(PokemonControllerTest,Get_All_Pokemon_Empty) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,getAllPokemon).WillOnce(testing::Return(std::vector<PokemonModel::Pokemon>{}));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/pokemon";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["pokemons"].size(), 0u);
	EXPECT_EQ(json["message"], "Total Pokemons Retrieved 0");
}

// Get Pokemon

TEST(PokemonControllerTest,Get_Pokemon_Success) {
	const MockPokemonRepo repo;
	const Router router;

	const PokemonModel::Pokemon fakePokemon{25,"pikachu"};
	EXPECT_CALL(repo,getPokemonById(25)).WillOnce(testing::Return(fakePokemon));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/pokemon/25";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["pokemon"].get<PokemonModel::Pokemon>(), fakePokemon);
	EXPECT_EQ(json["message"], "Pokemon found successfully");
}

TEST(PokemonControllerTest,Get_Pokemon_Not_Found) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,getPokemonById(999)).WillOnce(testing::Return(std::nullopt));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/pokemon/999";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::NOT_FOUND) << "Status not 404";
}

TEST(PokemonControllerTest,Get_Pokemon_Invalid_Id) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,getPokemonById(testing::_)).Times(0);
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::GET;
	request.route = "/api/pokemon/abc";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

// Update Pokemon

TEST(PokemonControllerTest,Update_Pokemon_Success) {
	const MockPokemonRepo repo;
	const Router router;

	const PokemonModel::Pokemon updated{5,"raichu"};
	EXPECT_CALL(repo,updatePokemon(testing::_)).WillOnce(testing::Return(updated));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/pokemon/5";
	request.body = R"({"pokemon": {"name": "raichu"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["pokemon"]["id"], 5);
	EXPECT_EQ(json["message"], "Pokemon updated successfully");
}

TEST(PokemonControllerTest,Update_Pokemon_No_Pokemon_Field) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,updatePokemon(testing::_)).Times(0);
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/pokemon/5";
	request.body = R"({"nope": true})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

TEST(PokemonControllerTest,Update_Pokemon_Not_Found) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,updatePokemon(testing::_)).WillOnce(testing::Return(std::nullopt));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/pokemon/5";
	request.body = R"({"pokemon": {"name": "raichu"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::NOT_FOUND) << "Status not 404";
}

TEST(PokemonControllerTest,Update_Pokemon_Invalid_Id) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,updatePokemon(testing::_)).Times(0);
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::PUT;
	request.route = "/api/pokemon/abc";
	request.body = R"({"pokemon": {"name": "raichu"}})";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}

// Delete Pokemon

TEST(PokemonControllerTest,Delete_Pokemon_Success) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,deletePokemonById(7)).WillOnce(testing::Return(true));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::DEL;
	request.route = "/api/pokemon/7";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::OK) << "Not ok status";
	nlohmann::json json = nlohmann::json::parse(response.body);
	EXPECT_EQ(json["message"], "Pokemon id 7 deleted successfully");
}

TEST(PokemonControllerTest,Delete_Pokemon_Not_Found) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,deletePokemonById(7)).WillOnce(testing::Return(false));
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::DEL;
	request.route = "/api/pokemon/7";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::NOT_FOUND) << "Status not 404";
}

TEST(PokemonControllerTest,Delete_Pokemon_Invalid_Id) {
	const MockPokemonRepo repo;
	const Router router;

	EXPECT_CALL(repo,deletePokemonById(testing::_)).Times(0);
	PokemonController controller(router,repo);

	ParsedRequestObject request{};
	request.method = Method::DEL;
	request.route = "/api/pokemon/abc";

	Response response {router.dispatch(request)};

	EXPECT_EQ(response.status,HttpStatus::BAD_REQUEST) << "Status not 400";
}
