#pragma once
#include "tournament.hpp"

class SingleElimination: public Tournament::System
{
public:
	std::expected<void, std::string> can_begin_tournament(Tournament::CompetitorsMap&) override;
    std::string_view get_description() override;
    Tournament::RoundNum get_rounds(Tournament::CompetitorsMap&) override;
    void prepare_pairings(Tournament::CompetitorsMap&, Tournament::Pairings&) override;
    void score_competitors(const Tournament::Pairings&, Tournament::Scores&, std::set<Tournament::CompetitorId>&) override;
private:
	std::set<Tournament::CompetitorId> eliminatedPlayers;
};