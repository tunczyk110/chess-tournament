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
    // types
    using CompetitorId = int;
    using RoundNum = size_t;

    struct Match {
        enum class Status {
            InProgress,
            WhiteWon,
            BlackWon,
            Drawn
        };

        CompetitorId white, black;
        size_t table_number;
        Status status;
    };

    using Pairings = std::vector<Match>;
    using Scores = std::map<CompetitorId, CompetitorResults>;
    using CompetitorsMap = std::map<CompetitorId, Competitor>;

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

    struct Status
    {
        State state;
        // if state isn't InProgress, values of following fields are meaningless
        RoundNum round_number;
        const Pairings& matches;
        const Scores& scores;
    };

    enum class ReportResult {
        Success,
        AlreadyReported,
        PlayerDroppedOut
    };

    // constants
    static constexpr CompetitorId COMP_BYE = -1;

    // constructor
    Tournament(std::unique_ptr<System>&& s):
        system{std::move(s)}
    {}

    // methods
    std::expected<void, std::string> begin_tournament();
    std::expected<State, std::string> complete_round();
    void add_competitors(std::span<const Competitor>);
    Status get_current_status() const { return {state.first, state.second, current_matches, competitor_points}; }
    const Competitor& get_competitor(CompetitorId id) const { return competitors.find(id)->second; }
    unsigned get_competitor_points(CompetitorId) const;

    ReportResult report_match(CompetitorId winner);

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

//class SingleElimination: public Tournament::System
//{
//
//};
