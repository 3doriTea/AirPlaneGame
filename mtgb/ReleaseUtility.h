#pragma once

// ポインタを安全に解放するためのマクロ
#define SAFE_DELETE(p)\
if (p != nullptr)      \
{                       \
	delete p;            \
	p = nullptr;          \
}

// ポインタ配列を安全に解放するためのマクロ
#define SAFE_DELETE_ARRAY(p)\
if (p != nullptr)            \
{                             \
	delete[] p;                \
	p = nullptr;                \
}

// ポインタを安全にReleaseするためのマクロ
#define SAFE_RELEASE(p)\
if (p != nullptr)       \
{                        \
	p->Release();         \
	p = nullptr;           \
}

// ポインタを安全にDestroyするためのマクロ
#define SAFE_DESTROY(p)\
if (p != nullptr)       \
{                        \
	p->Destroy();         \
	p = nullptr;           \
}

// コンテナを安全に解放するためのマクロ
// コンテナは std::vector や std::list などのこと
#define SAFE_DELETE_CONTAINER(container)\
{                                        \
	for (auto& p : container)             \
	{                                      \
		delete p;                           \
	}                                        \
	container.clear();                        \
}


