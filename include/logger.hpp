#pragma once
#include <format>
#include <iostream>

namespace Log {
    template<typename... Args>
    void println(std::string_view fmt, Args... args) {
        std::cout << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << "\n";
    }

    template<typename... Args>
    void debug(std::string_view fmt, Args... args) {
        std::cout << "[Debug]";
        println(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(std::string_view fmt, Args... args) {
        std::cout << "[Error]";
        println(fmt, std::forward<Args>(args)...);
    }
}
