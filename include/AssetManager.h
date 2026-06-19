#ifndef HTTPSERVER_ASSETMANAGER_H
#define HTTPSERVER_ASSETMANAGER_H
#include <string>
#include <unordered_map>
#include <vector>

class AssetManager {
    public:
    struct Asset {
        std::vector<char> data;
        std::string contentType;
    };
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    [[nodiscard]] const Asset& get(const std::string& assetName) const;
    void loadAsset(const std::string &path,const std::string& contentType ,const std::string &assetName) ;

private:
    std::unordered_map<std::string, Asset> assets_{};
    AssetManager()=default;

};


#endif //HTTPSERVER_ASSETMANAGER_H