#pragma once

#include <string>
#include <string_view>
#include <format>

class Competitor
{
public:
    Competitor(std::string_view fname, std::string_view lname):
        first_name{fname},
        last_name{lname}
    {}

    friend struct std::formatter<Competitor>;
private:
    std::string first_name, last_name;
};

template<>
struct std::formatter<Competitor> : std::formatter<std::string>
{
    auto format(const Competitor& c, std::format_context& ctx) const {
        return formatter<string>::format(std::format("{}, {}", c.last_name, c.first_name), ctx);
    }
};
