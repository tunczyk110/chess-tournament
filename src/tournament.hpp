#pragma once

#include <span>
#include <memory>
#include <vector>
#include <map>
#include <expected>
#include <set>

#include "competitor.hpp"

class Tournament
{
private:
    class System {
    public:
        virtual std::string_view get_description() = 0;
        virtual size_t get_rounds(std::span<const Competitor>) = 0;
        virtual void prepare_pairings(std::span<const Competitor>) = 0;

        virtual ~System() {}
    };
public:
    // types
    using CompetitorId = int;
    using RoundNum = size_t;
    using Points = unsigned;

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
    using Scores = std::map<CompetitorId, Points>;

    struct Status
    {
        RoundNum round_number;
        const Pairings& matches;
        const Scores& scores;
    };

    enum class State {
        Signups,
        InProgress,
        Finished
    };

    enum class ReportResult {
        Success,
        AlreadyReported,
        PlayerDroppedOut
    };

    class RoundRobin;

    // constants
    static constexpr CompetitorId COMP_BYE = -1;

    // constructor
    Tournament(std::unique_ptr<System>&& s):
        system{std::move(s)}
    {}

    // methods
    std::expected<void, std::string> begin_tournament() { return {}; }
    std::expected<State, std::string> complete_round() { return state.second == 1 ? State::Finished : State::InProgress; }
    void add_competitors(std::span<const Competitor>) {}
    Status get_current_status() { return {state.second, current_matches, competitor_points}; }
    const Competitor& get_competitor(CompetitorId id) { return competitors.find(id)->second; }
    void drop_out_competitor(CompetitorId) {}

    ReportResult report_match(CompetitorId /*winner*/) { return ReportResult::Success; }

private:
    std::unique_ptr<System> system;
    std::map<CompetitorId, Competitor> competitors;
    std::pair<State, RoundNum> state = {State::Signups, 0};
    std::vector<Match> current_matches;

    std::set<CompetitorId> out_of_tournament;
    std::map<CompetitorId, Points> competitor_points;
};

class Tournament::RoundRobin: public Tournament::System
{
public:
    std::string_view get_description() override { return "każdy z każdym"; };
    size_t get_rounds(std::span<const Competitor>) override { return 0; };
    void prepare_pairings(std::span<const Competitor>) override {};
private:
};

// class DoubleRoundRobin: public Tournament::System
// {

// };

// class Swiss: public Tournament::System
// {

// };

// class SingleElimination: public Tournament::System
// {

// };
