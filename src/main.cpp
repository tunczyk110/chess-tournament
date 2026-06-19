#include "tournament.hpp"
#include "competitor.hpp"

#include <print>
#include <iostream>

std::vector<Competitor> create_competitors()
{
    std::vector<Competitor> competitors;
    std::string fname, lname;
    while (true) {
        std::println("Podaj imię i nazwisko zawodnika (lub 'koniec' aby zakończyć): ");
        std::cin >> fname;
        if (fname == "koniec") break;
        std::cin >> lname;
        competitors.emplace_back(fname, lname);
		std::println("Dodano zawodnika.\n");
    }
    return competitors;
}

void tournament_options(Tournament& tour)
{
	bool running_tour = true;
    while (running_tour)
    {
        int choice;
        std::println("Dostępne opcje turnieju:\n1. Dodaj zawodników\n2. Rozpocznij turniej\n3. Zgłoś wynik\n4. Zakończ rundę\n5. Wyświetl aktualny status\n6. Przerwij turniej");
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
			if (tour.get_current_tour_state() == Tournament::State::InProgress) {
				std::println("Nie można dodawać zawodników po rozpoczęciu turnieju.");
				continue;
			}
            std::println("Wybrano opcję dodawania zawodników.");
            std::vector<Competitor> competitors = create_competitors();
            tour.add_competitors(competitors);
            tour.print_competitors();
            break;
        }
        case 2:
        {
            std::println("Wybrano opcję rozpoczęcia turnieju.");
            if (tour.get_competitor_count() == 0)
            {
				std::println("Nie można rozpocząć turnieju bez zawodników.");
				continue;
            }
            else if (tour.get_competitor_count() % 2 != 0)
            {
				std::println("Liczba zawodników musi być parzysta.");
                continue;
			}
			auto begin_result = tour.begin_tournament();
            if (!begin_result) {
                std::println("Błąd przy rozpoczęciu turnieju: {}", begin_result.error());
            }
                break;
        }
        case 3:
        {
            std::println("Wybrano opcję zgłaszania wyniku.");
            // tour.report_match(0); 
            break;
        }
        case 4:
        {
            std::println("Wybrano opcję zakończenia rundy.");
            // tour.complete_round();
            break;
        }
        case 5:
        {
            std::println("Wybrano opcję wyświetlenia aktualnego statusu.");
            break;
        }
        case 6:
        {
            std::println("Wybrano opcję zakończenia turnieju.");
            running_tour = false;
            break;
        }
        default:
            std::println("Niepoprawny wybór.");
        }
    }
    
}

int main()
{

    std::println("Zarządzanie turniejem szachowym - wybierz rodzaj rozgrywek:\n"
		"1. Każdy z każdym\n2. Pucharowy\n");
	int choice;
	std::cin >> choice;

    
	switch (choice) {
    case 1:
    {
        std::println("Wybrano rozgrywki typu każdy z każdym.");
        Tournament tour = { std::make_unique<Tournament::RoundRobin>() };
        tournament_options(tour);
        break;
    }
    case 2:
    {
        std::println("Wybrano rozgrywki pucharowe.");
        break;
    }
	default:
		std::println("Niepoprawny wybór.");
	}
    // tworzymy turniej typu każdy z każdym
    // Tournament tour{std::make_unique<Tournament::RoundRobin>()};
    /*
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
    */
    return 0;
}
