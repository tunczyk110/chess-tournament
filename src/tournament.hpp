#pragma once

#include <span>
#include <memory>
#include <vector>
#include <map>
#include <expected>
#include <set>

#include "competitor.hpp"

/**
 * Zawiera wyniki gracza w turnieju.
 */
struct CompetitorResults
{
    /**
     * Ilość wygranych rund.
     */
    size_t won = 0;

    /**
     * Ilość przegranych rund.
     */
    size_t lost = 0;

    /**
     * Ilość zremisowanych rund.
     */
    size_t drawn = 0;
};

class Tournament
{
public:
    /**
     * ID przypisane danemu zawodnikowi w turnieju.
     */
    using CompetitorId = int;

    /**
     * Typ oznaczający numer rozgrywanej rundy.
     */
    using RoundNum = size_t;

    /**
     * Struktura opisująca mecz w danej rundzie.
     */
    struct Match {

        /**
         * Enumeracja opisująca status meczu.
         */
        enum class Status {
            /**
             * Mecz jest w trakcie gry.
             */
            InProgress,
            /**
             * Wygrana białych.
             */
            WhiteWon,
            /**
             * Wygrana czarnych.
             */
            BlackWon,

            /**
             * Partia zremisowana.
             */
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
         * Status meczu.
         */
        Status status;
    };

    /**
     * Typ kontenera meczy rozgrywanych w rundzie.
     */
    using Pairings = std::vector<Match>;

    /**
     * Typ kontenera wyników graczy.
     */
    using Scores = std::map<CompetitorId, CompetitorResults>;

    /**
     * Typ kontenera zawodników turnieju.
     */
    using CompetitorsMap = std::map<CompetitorId, Competitor>;

    /**
     * Interfejs systemu organizacji turnieju.
     */
    class System {
    public:
        /**
         * Daje tekstowy opis danego systemu turniejowego.
         *
         * @return Widok na napis.
         */
        virtual std::string_view get_description() = 0;
        /**
         *  Podaje ilość rund przewidywanych przez system turniejowy dla danej ilości graczy.
         *
         * @param comp Kontener zawodników turnieju.
         * @return Całkowiwta ilość rund do rozegrania.
         */
        virtual RoundNum get_rounds(CompetitorsMap& comp) = 0;

        /**
         * Przygotowuje pary na następną rundę turnieju.
         *
         * @param[in] comp Kontener zawodników turnieju.
         * @param[out] par Kontener z przygotowanymi meczami na następną rundę.
         */
        virtual void prepare_pairings(CompetitorsMap& comp, Pairings& par) = 0;

        /**
         * Sprawdza, czy turniej jest w stanie umożliwiającym rozpoczęcie.
         *
         * @oaram comp Kontener zawodników turnieju.
         * @return void, jeśli turniej można rozpocząć. W przeciwnym razie std::string zawierający opis nieprawidłowości.
         */
        virtual std::expected<void, std::string> can_begin_tournament(CompetitorsMap& comp) = 0;


        /**
         * Zapisuje wyniki z danej rundy turniejowej. Wywoływana w procedurze zakończenia rundy.
         *
         * @oaram[in] par Kontener meczy rozegranych w danej rundzie.
         * @param[out] scores Kontener z punktacją graczy w turnieju.
         * @param[out] out Kontener z ID graczy wyeliminowanych z turnieju.
         */
        virtual void score_competitors(const Pairings& par, Scores& scores, std::set<CompetitorId>& out) = 0;


        /**
         * Domyślny destruktor wirtualny.
         */
        virtual ~System() = default;
    };


    /**
     * Enumeracja opisująca wartość maszyny stanów turnieju.
     */
    enum class State {

        /**
         * Turniej jest w fazie zapisów.
         */
        Signups,

        /**
         * Turniej jest w fazie rozgrywki.
         */
        InProgress,

        /**
         * Turniej został zakończony.
         */
        Finished
    };

    /**
     * Struktura opisująca obecny stan turnieju.
     */
    struct Status
    {
        /**
         * Obecna wartość maszyny stanów turnieju.
         */
        State state;

        /**
         * Numer obecnie rozgrywanej rundy.
         *
         * Jeśli `state` nie jest `InProgress`, wartość składnika jest niezdefiniowana.
         */
        RoundNum round_number;

        /**
         * Kontener meczy rozgrywanych w obecnej rundzie.
         *
         * Jeśli `state` nie jest `InProgress`, wartość składnika jest niezdefiniowana.
         */
        const Pairings& matches;


        /**
         * Kontener punktacji graczy w turnieju.
         *
         * Jeśli `state` jest `Signups`, wartość składnika jest niezdefiniowana.
         */
        const Scores& scores;
    };

    /**
     * Rezultat operacji zgłoszenia wyniku.
     */
    enum class ReportResult {
        /**
         * Pomyślnie zapisano wynik.
         */
        Success,

        /**
         * Wynik tego meczu został zgłoszony wcześniej.
         */
        AlreadyReported,

        /**
         * Podano nieprawidłowy numer stolika.
         */
        WrongTableNumber,

        /**
         * Podano nieprawidłowy rezultat partii.
         */
        WrongResultChar
    };

    /**
     * ID zawodnika używany w paringach przypadku "bye"
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
     * Turniej musi być rozpoczęty oraz niezakończony.
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
     * @return Enumeracja oznaczający rezultat operacji.
     */
    ReportResult report_match(size_t table_num, char result);


    /**
     * Wyświetla wyniki turnieju.
     */
    void print_results();

private:
    void track_results();

    std::unique_ptr<System> system;
    std::map<CompetitorId, Competitor> competitors;
    std::pair<State, RoundNum> state = {State::Signups, 0};
    Pairings current_matches;

    std::set<CompetitorId> out_of_tournament;
    Scores competitor_points;
};
