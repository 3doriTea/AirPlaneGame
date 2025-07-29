#pragma once
#include <tuple>
#include <vector>
#include <functional>
#include <type_traits>
#include <map>
#include "RenderResource.h"
typedef struct HWND__* HWND;

template <class... Args>
class RenderResourceManager
{
public:
	using ResourceTuple = std::tuple<Args*...>;
	RenderResourceManager();

	void CreateRenderResource(HWND _hWnd);
	template<std::size_t I>
	void InitializeResource(ResourceTuple& tuple, HWND _hWnd);
	template<std::size_t... Is>
	void InitializeSequentially(ResourceTuple& _tuple, HWND _hWnd, std::index_sequence<Is...>)
	{
		(InitializeResource<Is>(_tuple, _hWnd), ...);
	}
private:
	std::map<HWND,ResourceTuple> resources_;
};

template<class ...Args>
inline RenderResourceManager<Args...>::RenderResourceManager()
{
	static constexpr bool allAreBaseOfRenderResource =
		(std::is_base_of_v<RenderResource, Args> && ...);

	static_assert(allAreBaseOfRenderResource
		&& "RenderResource�N���X���p�����Ă��Ȃ��N���X�̓C���X�^���X�ł��܂���B");

}

template<class ...Args>
inline void RenderResourceManager<Args...>::CreateRenderResource(HWND _hWnd)
{
	ResourceTuple tuple{};
	InitializeSequentially(tuple, _hWnd, std::index_sequence_for<Args...>{});
	resources_.insert(std::make_pair(_hWnd,std::move(tuple));
}

template<class ...Args>
template<std::size_t I>
inline void RenderResourceManager<Args...>::InitializeResource(ResourceTuple& tuple, HWND _hWnd)
{
	//I�Ԗڂ̌^���擾
	using ResourceType = std::tuple_element_t<I, std::tuple<Args...>>;

	//new�ō쐬
	std::get<I>(tuple) = new ResourceType;

	std::get<I>(tuple)->Initialize(tuple, _hWnd);
}
