#pragma once

#include <string>
#include <string_view>
#include <format>

/**
 * Zawiera dane zawodnika turnieju.
 */
class Competitor
{
public:

    /**
     * @param fname Imię zawodnika.
     * @param lname Nazwisko zawodnika.
     */
    Competitor(std::string_view fname, std::string_view lname):
        first_name{fname},
        last_name{lname}
    {}

    /**
     * Struktura formatująca dla `std::format`
     */
    friend struct std::formatter<Competitor>;
private:
    std::string first_name, last_name;
};

/**
 * Struktura formatująca dla `std::format`
 */
template<>
struct std::formatter<Competitor> : std::formatter<std::string>
{
    auto format(const Competitor& c, std::format_context& ctx) const {
        return formatter<string>::format(std::format("{}, {}", c.last_name, c.first_name), ctx);
    }
};
