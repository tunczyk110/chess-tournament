#include "singleElimination.hpp"

#include <random>

std::expected<void, std::string> SingleElimination::can_begin_tournament(Tournament::CompetitorsMap& competitors)
{
	if (competitors.size() < 2) {
		return std::unexpected("za mało zawodników");
	}
	else if ((competitors.size() & (competitors.size() - 1)) != 0) {
		return std::unexpected("liczba zawodników w turnieju pucharowym musi być potęgą dwójki");
	}
	return {};
}

std::string_view SingleElimination::get_description()
{
	return "pucharowy";
}

void SingleElimination::prepare_pairings(Tournament::CompetitorsMap& competitors, Tournament::Pairings& pairings)
{
	pairings.clear();
	std::vector<Tournament::CompetitorId> active_comps;
	for (const auto& [id, comp] : competitors)
	{
		if (!eliminatedPlayers.contains(id))
		{
			active_comps.push_back(id);
		}
	}
	std::shuffle(active_comps.begin(), active_comps.end(), std::mt19937{ std::random_device{}() });
	for (size_t i = 0; i < active_comps.size(); i += 2)
	{
		pairings.push_back({ active_comps[i], active_comps[i + 1], static_cast<unsigned>(i / 2 + 1), Tournament::Match::Status::InProgress });
	}
}

void SingleElimination::score_competitors(const Tournament::Pairings& current_matches, Tournament::Scores&, std::set<Tournament::CompetitorId>& eliminated)
{
	for (const auto& m : current_matches) {
		if (m.status == Tournament::Match::Status::WhiteWon) {
			eliminated.insert(m.black);
		}
		else if (m.status == Tournament::Match::Status::BlackWon) {
			eliminated.insert(m.white);
		}
	}
	eliminatedPlayers = eliminated;
}

Tournament::RoundNum SingleElimination::get_rounds(Tournament::CompetitorsMap& comps)
{
	return static_cast<Tournament::RoundNum>(std::log2(comps.size()));
}
