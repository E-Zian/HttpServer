#pragma once
#ifndef Helper_H
#define Helper_H

#include <fmt/color.h>
#include <string>
#include <algorithm>
#include <vector>
#include <optional>

namespace Helper {
	std::string trim(std::string text);

	template<typename... Args>
	void displayError(fmt::format_string<Args...> fmt_string, Args &&... args) {
		fmt::print(fmt::fg(fmt::color::red), "Server Error : ");
		fmt::print("{}\n\n", fmt::format(fmt_string, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void displayMessage(fmt::format_string<Args...> fmt_str, Args &&... args) {
		fmt::print(fmt::fg(fmt::color::green), "Server Info : ");
		fmt::print("{}\n\n", fmt::format(fmt_str, std::forward<Args>(args)...));
	}

	std::vector<std::string> split(const std::string& text, const char delimiter);

}

#endif 
