#include "Helper.h"

#include <sstream>

std::string Helper::trim(std::string text) {
    text.erase(text.begin(), std::ranges::find_if(text, [](const unsigned char c) {
                                                      return !std::isspace(c);
                                                  }
               ));
    text.erase(std::find_if(text.rbegin(), text.rend(), [](const unsigned char c) {
        return !std::isspace(c);
    }).base(), text.end());

    return text;
}

std::vector<std::string> Helper::split(const std::string& text, const char delimiter) {
    std::vector<std::string> stringComponents{};

    std::stringstream ss(text);

    std::string component;
    while (std::getline(ss,component,delimiter)) {
        if (!component.empty()) {
            stringComponents.push_back(component);
        }
    };

    return stringComponents;
}

