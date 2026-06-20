#include <print>
#include <iostream>

#include "tournament.hpp"
#include "competitor.hpp"

#include "round_robin.hpp"
#include "singleElimination.hpp"

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
        std::println("**************************************\nDostępne opcje turnieju:\n1. Dodaj zawodników\n2. Rozpocznij turniej\n3. Zgłoś wynik\n4. Zakończ rundę\n5. Wyświetl aktualny status\n6. Przerwij turniej");
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
			if (tour.get_current_status().state != Tournament::State::Signups) {
				std::println("Nie można dodawać zawodników po rozpoczęciu turnieju.");
				continue;
			}
            tour.add_competitors(create_competitors());
            break;
        }
        case 2:
        {
			auto begin_result = tour.begin_tournament();
            if (!begin_result) {
                std::println("Błąd przy rozpoczęciu turnieju: {}", begin_result.error());
                break;
            }
            std::println("Turniej rozpoczęty! Zaczyna się runda 1:");
            for (const auto& m: tour.get_current_status().matches) {
                std::println("\t[{}]: {} (białe) vs {} (czarne)", m.table_number, tour.get_competitor(m.white), tour.get_competitor(m.black));
            }
            break;
        }
        case 3:
        {
            if (tour.get_current_status().state != Tournament::State::InProgress) {
                std::println("Nie można zgłaszać wyników, gdy nie jesteśmy w trakcie rundy.");
                break;
            }
            std::print("Podaj ID gracza który wygrał: ");
            Tournament::CompetitorId id;
            std::cin >> id;
            auto report_result = tour.report_match(id);
            switch (report_result) {
            case Tournament::ReportResult::AlreadyReported:
                std::println("Wynik tego meczu już został zgłoszony.");
                break;
            case Tournament::ReportResult::PlayerDroppedOut:
                std::println("Ten gracz odpadł już z turnieju.");
                break;
            case Tournament::ReportResult::Success:
                std::println("Sukces");
                break;
            }
            break;
        }
        case 4:
        {
            auto complete_result = tour.complete_round();
            if (!complete_result) {
                std::println("Błąd przy zakończeniu rundy: {}", complete_result.error());
                break;
            }
            if (complete_result.value() == Tournament::State::Finished) {
                std::println("To była ostatnia runda. Wyniki końcowe:");
				tour.print_results();
            } else {
                std::println("Zakończono rundę {}. Rozpoczyna się runda {}", tour.get_current_status().round_number-1, tour.get_current_status().round_number);
                for (const auto& m: tour.get_current_status().matches) {
                    std::println("\t[{}]: {} (białe) vs {} (czarne)", m.table_number, tour.get_competitor(m.white), tour.get_competitor(m.black));
                }
            }
            break;
        }
        case 5:
        {
            auto status = tour.get_current_status();
            switch(status.state) {
            case Tournament::State::Signups:
                std::println("Trwają zapisy zawodników. Do tej pory zapisano {} zawodników:", tour.get_current_status().scores.size());
                for (auto [comp, score]: tour.get_current_status().scores) {
                    std::println("\t{}", tour.get_competitor(comp));
                }
                break;
            case Tournament::State::Finished:
                std::println("Turniej został zakończony.");
                tour.print_results();
                break;
            case Tournament::State::InProgress:
                std::println("Runda {}", status.round_number);
                for (const auto& m: status.matches) {
                    std::print("\t[{}]: {} (białe) vs {} (czarne) - ", m.table_number, tour.get_competitor(m.white), tour.get_competitor(m.black));
                    switch(m.status) {
                    case Tournament::Match::Status::InProgress:
                        std::println("w trakcie");
                        break;
                    case Tournament::Match::Status::WhiteWon:
                        std::println("wygrały białe");
                        break;
                    case Tournament::Match::Status::BlackWon:
                        std::println("wygrały czarne");
                        break;
                    case Tournament::Match::Status::Drawn:
                        std::println("remis");
                        break;
                    }
                }
            }
            break;
        }
        case 6:
        {
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
        Tournament tour{ std::make_unique<RoundRobin>() };
        tournament_options(tour);
        break;
    }
    case 2:
    {
        std::println("Wybrano rozgrywki pucharowe.");
        Tournament tour{ std::make_unique<SingleElimination>() };
        tournament_options(tour);
        break;
    }
	default:
		std::println("Niepoprawny wybór.");
	}
    return 0;
}
