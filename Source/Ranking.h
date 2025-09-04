#pragma once
#include <mtgb.h>
#include <vector>
#include "../mtgb/MTBin/MemoryStream.h"

class Ranking
{
public:
	Ranking();
	~Ranking();

	/// <summary>
	/// <para>ランキングの更新</para>
	/// <para>新しいエントリーを追加し、ランキングをソートして最大数に収める</para>
	/// </summary>
	void UpdateRanking(std::vector<int>& _rankingList, const int& newEntry);

	/// <summary>
	/// 指定されたファイルをメモリストリームに読み込みます。
	/// </summary>
	void LoadFileToMemoryStream(const char* _fileName, mtbin::MemoryStream& ms);

	/// <summary>
	/// メモリストリームの内容を指定されたファイルに保存します。
	/// </summary>
	void SaveMemoryStreamToFile(const char* _fileName, mtbin::MemoryStream& ms, size_t _dataSize);

	// ゲッター
	std::vector<int>& GetRankingList(){ return rankingList_; }

private:
	std::vector<int> rankingList_;
};