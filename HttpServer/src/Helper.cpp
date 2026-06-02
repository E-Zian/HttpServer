#include "Helper.h"

std::string Helper::trim(std::string text) {
    text.erase(text.begin(), std::find_if(text.begin(), text.end(), [](unsigned char c) {
                                              return !std::isspace(c);
                                          }
               ));
    text.erase(std::find_if(text.rbegin(), text.rend(), [](unsigned char c) {
        return !std::isspace(c);
    }).base(), text.end());

    return text;
}

// For runtime issue
void Helper::displayMessage(std::string_view msg) {
    fmt::print(fmt::fg(fmt::color::green), "Server Info : ");
    fmt::print("{}\n", msg);
}
