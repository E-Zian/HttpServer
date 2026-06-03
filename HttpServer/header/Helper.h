#pragma once
#ifndef Helper_H
#define Helper_H
#include <string>
#include <algorithm>
#include <fmt/color.h>
namespace Helper {
	std::string trim(std::string text);

	template<typename... Args>
	void displayError(fmt::format_string<Args...> fmt_string, Args &&... args) {
		fmt::print(fmt::fg(fmt::color::red), "Server Error : ");
		fmt::print("{}\n", fmt::format(fmt_string, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void displayMessage(fmt::format_string<Args...> fmt_str, Args &&... args) {
		fmt::print(fmt::fg(fmt::color::green), "Server Info : ");
		fmt::print("{}\n", fmt::format(fmt_str, std::forward<Args>(args)...));
	}


}

#endif 
