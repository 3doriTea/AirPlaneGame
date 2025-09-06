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

void Ranking::UpdateRanking(std::vector<int>& _rankingList, const int& _newEntry)
{
	_rankingList.push_back(_newEntry);
	std::sort(_rankingList.begin(), _rankingList.end(), 
		[](auto& a, auto& b) { return a > b; });
	if (_rankingList.size() > maxRankingCount)
	{
		_rankingList.resize(maxRankingCount);
	}
}

void Ranking::LoadFileToMemoryStream(const char* _fileName, mtbin::MemoryStream& _ms)
{
    FILE* file;
    fopen_s(&file, _fileName, "rb");
    if (!file) return;

    fread(_ms.GetBuffer(), 1, _ms.GetLength(), file);
    fclose(file);

    // Å©Ç±Ç±Ç≈msÇ©ÇÁíºê⁄rankingList_Çïúå≥
    _ms.Seek(mtbin::MemoryStream::SeekDir::Head);
    rankingList_.resize(maxRankingCount);
    _ms.Read(rankingList_.data(), (int)rankingList_.size(), (int)rankingList_.size());
}

void Ranking::SaveMemoryStreamToFile(const char* _fileName, mtbin::MemoryStream& _ms, size_t _dataSize)
{
    FILE* file;
    fopen_s(&file, _fileName, "wb");
    if (!file) return;

    fwrite(_ms.GetBuffer(), 1, _dataSize, file);
    fclose(file);
}