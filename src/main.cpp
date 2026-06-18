#include "tournament.hpp"
#include "competitor.hpp"

#include <print>

int main()
{
    // tworzymy turniej typu każdy z każdym
    Tournament tour{std::make_unique<Tournament::RoundRobin>()};

    // przed rozpoczęciem turnieju dodajemy zawodników
    Competitor comp1{"Andrzej", "Awokado"};
    Competitor comp2{"Beata", "Bulwa"};
    Competitor comp3{"Cezary", "Cebula"};
    Competitor comp4{"Dariusz", "Daleki"};

    std::println("Print test: dwoje pierwszych zawodników to {} i {}.", comp1, comp2);
    tour.add_competitors({{comp1, comp2, comp3, comp4}});

    // po dodaniu zawodników rozpoczynamy turniej
    auto begin_result = tour.begin_tournament();
    if (!begin_result) {
        std::print("Błąd przy rozpoczęciu turnieju: {}", begin_result.error());
    }
    auto pairings = tour.get_current_status().matches;

    // zgłaszamy wyniki
    tour.report_match(0);
    tour.report_match(2);

    // po zgłoszeniu wszystkich wyników zakańczamy rundę
    auto complete_result = tour.complete_round();
    if (!complete_result) {
        std::print("Błąd przy rozpoczęciu turnieju: {}", begin_result.error());
    }
    auto new_state = complete_result.value();
    if (new_state== Tournament::State::Finished) {
        // koniec turnieju, wyświetlamy ostateczne wyniki
        auto scores = tour.get_current_status().scores;
    } else {
        // nowa runda, bierzemy nowe paringi
        auto pairings = tour.get_current_status().matches;
    }

    return 0;
}
