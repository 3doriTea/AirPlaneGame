#include "HLSLInclude.h"
#include <string>
#include <fstream>

HRESULT mtgb::HLSLInclude::Open(
	D3D_INCLUDE_TYPE _includeType,
	LPCSTR _pFileName,
	LPCVOID _pParentData,
	LPCVOID* _ppData,
	UINT* _pBytes)
{
	std::string fileName{ _pFileName };
	std::string headersDirectory{ "../Assets/Shader/" };

	std::ifstream ifs{ headersDirectory + fileName, std::ios::binary };
	ifs.seekg(0, std::ios::end);
	*_pBytes = ifs.tellg();

	*_ppData = new char[*_pBytes];

	ifs.seekg(0, std::ios::beg);

	// バッファを動的確保して読み取る
	char* buffer{ const_cast<char*>(static_cast<const char*>(*_ppData)) };
	ifs.read(buffer, *_pBytes);

	return S_OK;
}

HRESULT mtgb::HLSLInclude::Close(LPCVOID _pData)
{
	// 解放
	delete[] static_cast<const char*>(_pData);

	return S_OK;
}
