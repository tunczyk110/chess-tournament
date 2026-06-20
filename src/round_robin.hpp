#pragma once

#include <map>
#include <set>

#include "tournament.hpp"

class RoundRobin: public Tournament::System
{
public:
    std::string_view get_description() override { return "każdy z każdym"; };
    size_t get_rounds(Tournament::CompetitorsMap&) override;
    void prepare_pairings(Tournament::CompetitorsMap&, Tournament::Pairings&) override;
    std::expected<void, std::string> can_begin_tournament(Tournament::CompetitorsMap&) override;
    void score_competitors(const Tournament::Pairings&, Tournament::Scores&, std::set<Tournament::CompetitorId>&) override {};
private:
    std::map<Tournament::CompetitorId, std::set<Tournament::CompetitorId>> remaining_matchups;
};
