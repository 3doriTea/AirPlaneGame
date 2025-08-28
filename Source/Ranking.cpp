#include "Ranking.h"
#include <fstream>
#include "MTAssert.h"

namespace
{
	size_t maxRankingCount{ 5 };
};

Ranking::Ranking()
{
}

Ranking::~Ranking()
{
}

void Ranking::UpdateRanking(std::vector<int>& _rankingList, const int& newEntry)
{
	_rankingList.push_back(newEntry);
	std::sort(_rankingList.begin(), _rankingList.end(), 
		[](auto& a, auto& b) { return a > b; });
	if (_rankingList.size() > maxRankingCount)
	{
		_rankingList.resize(maxRankingCount);
	}
}

void Ranking::LoadFileToMemoryStream(const char* fileName, mtbin::MemoryStream& ms)
{
    FILE* file;
    fopen_s(&file, fileName, "rb");
    if (!file) return;

    fread(ms.GetBuffer(), 1, ms.GetLength(), file);
    fclose(file);

    // Å©Ç±Ç±Ç≈msÇ©ÇÁíºê⁄rankingList_Çïúå≥
    ms.Seek(mtbin::MemoryStream::SeekDir::Head);
    rankingList_.resize(maxRankingCount);
    ms.Read(rankingList_.data(), (int)rankingList_.size(), (int)rankingList_.size());
}

void Ranking::SaveMemoryStreamToFile(const char* fileName, mtbin::MemoryStream& ms, size_t dataSize)
{
    FILE* file;
    fopen_s(&file, fileName, "wb");
    if (!file) return;

    fwrite(ms.GetBuffer(), 1, dataSize, file);
    fclose(file);
}