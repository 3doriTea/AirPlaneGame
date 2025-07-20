#pragma once

#define SAFE_DELETE(p)\
if (p != nullptr)      \
{                       \
	delete p;            \
	p = nullptr;          \
}

#define SAFE_DELETE_ARRAY(p)\
if (p != nullptr)            \
{                             \
	delete[] p;                \
	p = nullptr;                \
}

#define SAFE_RELEASE(p)\
if (p != nullptr)       \
{                        \
	p->Release();         \
	p = nullptr;           \
}

#define SAFE_DELETE_CONTAINER(container)\
{                                        \
	for (auto& p : container)             \
	{                                      \
		delete p;                           \
	}                                        \
	container.clear();                        \
}
