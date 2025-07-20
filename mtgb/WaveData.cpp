#include "WaveData.h"
#include "ReleaseUtility.h"

mtgb::WaveData::WaveData() :
	waveFormat{},
	pBuffer{ nullptr },
	bufferSize{ 0 }
{
}

mtgb::WaveData::~WaveData()
{
	SAFE_DELETE_ARRAY(pBuffer);
}
