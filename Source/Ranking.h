#pragma once
#include <mtgb.h>
#include "../mtgb/MTBin/MemoryStream.h"

class Ranking
{
public:
	Ranking();
	~Ranking();
	void UpdateRanking(std::vector<ScoreEntry>)
	void LoadFileToMemoryStream(const char* _fileName, mtbin::MemoryStream& ms);
	void SaveMemoryStreamToFile(const char* _fileName, mtbin::MemoryStream& ms);
};