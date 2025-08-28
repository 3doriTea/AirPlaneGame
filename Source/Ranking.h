#pragma once
#include <mtgb.h>
#include <vector>
#include "../mtgb/MTBin/MemoryStream.h"

class Ranking
{
public:
	Ranking();
	~Ranking();
	void UpdateRanking(std::vector<int>& _rankingList, const int& newEntry);
	void LoadFileToMemoryStream(const char* _fileName, mtbin::MemoryStream& ms);
	void SaveMemoryStreamToFile(const char* _fileName, mtbin::MemoryStream& ms, size_t _dataSize);

	// ÉQÉbÉ^Å[
	std::vector<int>& GetRankingList(){ return rankingList_; }

private:
	std::vector<int> rankingList_;
};