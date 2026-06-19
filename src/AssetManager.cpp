#include "AssetManager.h"

#include <fstream>

#include "Helper.h"

const AssetManager::Asset& AssetManager::get(const std::string &assetName) const {
    const auto it {assets_.find(assetName)};
    if (it == assets_.end()) {
        Helper::displayError("asset {} not found", assetName);
    }
    return it->second;
}

void AssetManager::loadAsset(const std::string &path,const std::string& contentType ,const std::string &assetName) {
    std::ifstream file(path,std::ios::binary | std::ios::end);

    if (!file.is_open()) {
        Helper::displayError("cant load {} ",path);
    }

    const std::streamsize size {file.tellg()};
    file.seekg(0,std::ios::beg);

    std::vector<char> buffer(size);

    file.read(buffer.data(),size);

    file.close();

    assets_[assetName] = {buffer,contentType};
}
