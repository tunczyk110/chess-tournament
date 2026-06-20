#include "round_robin.hpp"

#include <algorithm>

size_t RoundRobin::get_rounds(Tournament::CompetitorsMap& comps)
{
    return comps.size() - 1;
}
void RoundRobin::prepare_pairings(Tournament::CompetitorsMap& comps, Tournament::Pairings& matches)
{
    matches.clear();

    // first round pairings
    if (remaining_matchups.size() == 0) {
        for (const auto&[comp_id, _]: comps) {
            auto& opponents_set = remaining_matchups[comp_id];
            for (const auto&[opp_id, _]: comps) {
                if (opp_id != comp_id) opponents_set.insert(opp_id);
            }
        }
    }

    std::set<Tournament::CompetitorId> left_to_pair_this_round;
    std::ranges::transform(comps, std::inserter(left_to_pair_this_round, left_to_pair_this_round.end()), [] (auto& c) { return c.first; });

    size_t table = 0;
    for (auto comp_it = left_to_pair_this_round.begin(); comp_it != left_to_pair_this_round.end();) {
        Tournament::CompetitorId opp_id = 0;
        for (auto id: remaining_matchups[*comp_it]) {
            if (left_to_pair_this_round.find(id) != left_to_pair_this_round.end()) {
                opp_id = id;
                break;
            }
        }
        matches.emplace_back(*comp_it, opp_id, table++, Tournament::Match::Status::InProgress);

        remaining_matchups[opp_id].erase(*comp_it);
        remaining_matchups[*comp_it].erase(opp_id);

        left_to_pair_this_round.erase(opp_id);
        left_to_pair_this_round.erase(*(comp_it++));
    }
}

std::expected<void, std::string> RoundRobin::can_begin_tournament(Tournament::CompetitorsMap& comps)
{
    if (comps.size() % 2 != 0) {
        return std::unexpected{"turniej \"każdy z każdym\" wymaga parzystej ilości uczestników"};
    }
    return {};
}

void RoundRobin::score_competitors(const Tournament::Pairings& matches, Tournament::Scores& scores, std::set<Tournament::CompetitorId>&)
{
    for (const auto& m: matches) {
        if (m.status == Tournament::Match::Status::WhiteWon) {
            scores[m.white].won += 1;
        } else if (m.status == Tournament::Match::Status::BlackWon) {

        }
        switch (m.status) {
        case Tournament::Match::Status::WhiteWon:
            scores[m.white].won += 1;
            scores[m.black].lost += 1;
            break;
        case Tournament::Match::Status::BlackWon:
            scores[m.black].won += 1;
            scores[m.white].lost += 1;
            break;
        case Tournament::Match::Status::Drawn:
            scores[m.black].drawn += 1;
            scores[m.white].drawn += 1;
            break;
        default:
            throw std::runtime_error{std::format("unexpected match status when scoring {} vs {}; state is {}", m.white, m.black, int(m.status))};
        }
    }
}
