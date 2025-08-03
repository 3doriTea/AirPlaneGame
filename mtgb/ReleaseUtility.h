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

// コンテナの要素を安全にReleaseするためのマクロ
// コンテナは std::vector や std::list などのこと
#define SAFE_CLEAR_CONTAINER_RELEASE(container)\
{                                                \
	for (auto& p : container)                     \
	{                                              \
		SAFE_RELEASE(p);                            \
	}                                                \
	container.clear();                                \
}

// コンテナの要素を安全に解放するためのマクロ
// コンテナは std::vector や std::list などのこと
#define SAFE_CLEAR_CONTAINER_DELETE(container)\
{                                               \
	for (auto& p : container)                    \
	{                                             \
		SAFE_DELETE(p);                            \
	}                                               \
	container.clear();                               \
}


