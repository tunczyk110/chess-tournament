#pragma once

#include <span>
#include <memory>
#include <vector>
#include <map>
#include <expected>
#include <set>

#include "competitor.hpp"

struct CompetitorResults
{
    size_t won = 0, lost = 0, drawn = 0;
};

class Tournament
{
public:
    using CompetitorId = int;
    using RoundNum = size_t;

    /**
     * Struktura opisująca mecz w danej rundzie.
     */
    struct Match {
        enum class Status {
            InProgress,
            WhiteWon,
            BlackWon,
            Drawn
        };

        /**
         * ID zawodnika grającego białymi.
         */
        CompetitorId white;

        /**
         * ID zawodnika grającego czarnymi.
         */
        CompetitorId black;

        /**
         * Numer stolika przy którym rozegrany zostanie mecz.
         */
        size_t table_number;

        /**
         * Numer stolika przy którym rozegrany zostanie mecz.
         */
        Status status;
    };

    using Pairings = std::vector<Match>;
    using Scores = std::map<CompetitorId, CompetitorResults>;
    using CompetitorsMap = std::map<CompetitorId, Competitor>;

    /**
     * Interfejs systemu organizacji turnieju.
     */
    class System {
    public:

        virtual std::string_view get_description() = 0;
        virtual RoundNum get_rounds(CompetitorsMap&) = 0;
        virtual void prepare_pairings(CompetitorsMap&, Pairings&) = 0;
        virtual std::expected<void, std::string> can_begin_tournament(CompetitorsMap&) = 0;
        virtual void score_competitors(const Pairings&, Scores&, std::set<CompetitorId>&) = 0;

        virtual ~System() {}
    };

    enum class State {
        Signups,
        InProgress,
        Finished
    };

    /**
     * Struktura opisująca obecny stan turnieju.
     */
    struct Status
    {
        State state;
        // if state isn't InProgress, values of following fields are meaningless
        RoundNum round_number;
        const Pairings& matches;
        const Scores& scores;
    };

    /**
     * Specjalny ID zawodnika używany w paringach przypadku "bye"
     */
    enum class ReportResult {
        Success,
        AlreadyReported,
        PlayerDroppedOut,
        WrongTableNumber,
        WrongResultChar
    };

    /**
     * Specjalny ID zawodnika używany w paringach przypadku "bye"
     */
    static constexpr CompetitorId COMP_BYE = -1;

    /**
     * Konstruktor klasy.
     *
     * @param system System organizacji dla danego turnieju.
     */
    explicit Tournament(std::unique_ptr<System>&& system):
        system{std::move(system)}
    {}

    /**
     * Metoda rozpoczyna turniej i przygotowuje paringi na rundę pierwszą.
     *
     * Turniej nie może być już rozpoczęty, i zapisana musi być odpowiednia ilość graczy dla danego systemu turniejowego.
     *
     * @return void, jeśli operacja przebiegła pomyślnie. W przeciwnym wypadku std::string zawierający opis błędu.
     */
    std::expected<void, std::string> begin_tournament();

    /**
     * Metoda zakańcza obecną rundę.
     *
     * Turniej nie może być już rozpoczęty, i zapisana musi być odpowiednia ilość graczy dla danego systemu turniejowego.
     *
     * @return Nowy stan turnieju, jeśli operacja przebiegła pomyślnie: InProgress, jeśli rozpoczęła się kolejna runda, lub Finished, jeśli była to runda ostatnia. W przypadku błędu std::string zawierający opis błędu.
     */
    std::expected<State, std::string> complete_round();

    /**
     * Metoda zapisuje zawodników do nowego turnieju.
     *
     * Turniej nie może być już rozpoczęty.
     *
     * @param competitors Lista zawodników do zapisania.
     */
    void add_competitors(std::span<const Competitor> competitors);

    /**
     * Metoda zwraca obecny stan turnieju.
     *
     * Można ją wywołać w dowolnym momencie.
     *
     * @return Obecny stan turnieju.
     */
    Status get_current_status() const { return {state.first, state.second, current_matches, competitor_points}; }

    /**
     * Metoda zwraca strukturę z danymi zawodnika o podanym ID.
     *
     * Można ją wywołać w dowolnym momencie.
     *
     * @param id ID zawodnika.
     * @return Struktura zawierająca dane zawodnika.
     */
    const Competitor& get_competitor(CompetitorId id) const { return competitors.find(id)->second; }

    /**
     * Metoda zwraca ilość punktów uzyskanych przez danego zawodnika w turnieju.
     *
     * Można ją wywołać w dowolnym momencie. Jeśli jest wywołana przed zakończeniem turnieju, wynik z bieżącej rundy nie jest brany pod uwagę.
     *
     * @param id ID zawodnika.
     * @return Struktura zawierająca dane zawodnika.
     */
    unsigned get_competitor_points(CompetitorId) const;

    /**
     * Metoda zapisuje wynik meczu.
     *
     * Może być wywołana tylko w trakcie turnieju, przed zakończeniem ostatniej rundy.
     *
     * @param table_num Numer stolika identyfikujący mecz.
     * @param result Znak oznaczający wynik do zapisania: 'B' oznacza wygranę białych, 'C' oznacza wygraną czarnych, 'R' oznacza remis.
     * @return Enum oznaczający rezultat operacji.
     */
    ReportResult report_match(size_t table_num, char result);

    void print_results();

private:
    std::unique_ptr<System> system;
    std::map<CompetitorId, Competitor> competitors;
    std::pair<State, RoundNum> state = {State::Signups, 0};
    Pairings current_matches;

    std::set<CompetitorId> out_of_tournament;
    Scores competitor_points;
};

// class DoubleRoundRobin: public Tournament::System
// {

// };

// class Swiss: public Tournament::System
// {

// };
