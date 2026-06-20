#include "tournament.hpp"

#include <stdexcept>

std::expected<void, std::string> Tournament::begin_tournament()
{
    if (auto begin_res = system->can_begin_tournament(competitors); !begin_res) {
        return std::unexpected(std::format("nie można rozpocząć turnieju: {}", begin_res.error()));
    }
    auto&[state, round_num] = this->state;
    state = State::InProgress;
    round_num = 1;
    system->prepare_pairings(competitors, current_matches);

    return {};
}

std::expected<Tournament::State, std::string> Tournament::complete_round()
{
    auto& [state, round_num] = this->state;

    // check if tournament is in progress
    if (state == State::Signups) {
        return std::unexpected("turniej nie został jeszcze rozpoczęty");
    } else if (state == State::Finished) {
        return std::unexpected("turniej został już zakończony");
    }

    // check if any matches are still in progress
    if (std::ranges::find_if(current_matches, [] (auto& m) {
        return m.status == Match::Status::InProgress;
    }) != current_matches.cend()) {
        return std::unexpected("nie wszystkie mecze są zakończone");
    }

    system->score_competitors(current_matches, competitor_points, out_of_tournament);

    // check if this round is the last round of tournament
    if (round_num == system->get_rounds(competitors)) {
        state = State::Finished;
    } else {
        ++round_num;
        system->prepare_pairings(competitors, current_matches);
    }

    return state;
}

void Tournament::add_competitors(std::span<const Competitor> new_comps)
{
    CompetitorId id = competitors.size() == 0 ? 0 : competitors.rbegin()->first + 1;
    for (const auto& c: new_comps) {
        auto new_id = id++;
        competitors.insert({new_id, c});
        competitor_points.insert({new_id, {0, 0, 0}});
    }
}

auto Tournament::report_match(CompetitorId winner) -> ReportResult
{
    auto match_it = std::ranges::find_if(current_matches, [winner] (auto& m) {
        return m.white == winner || m.black == winner;
    });
    if (match_it == current_matches.end()) {
        throw std::invalid_argument{"nieprawidłowy ID zawodnika"};
    }

    if (match_it->status != Match::Status::InProgress) {
        return ReportResult::AlreadyReported;
    }

    if (match_it->white == winner) {
        match_it->status = Match::Status::WhiteWon;
    } else {
        match_it->status = Match::Status::BlackWon;
    }

    return ReportResult::Success;
}

unsigned Tournament::get_competitor_points(CompetitorId comp_id) const
{
    auto comp_it = competitor_points.find(comp_id);
    if (comp_it == competitor_points.cend()) {
        throw std::invalid_argument{"nieprawidłowy ID zawodnika"};
    }
    auto&[_, comp] = *comp_it;

    return 2*comp.won + comp.drawn;
}
