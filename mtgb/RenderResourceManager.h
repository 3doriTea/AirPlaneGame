#pragma once
#include <tuple>
#include <vector>
#include <functional>
#include <type_traits>
#include "RenderResource.h"
template <class... Args>
class RenderResourceManager
{
	RenderResourceManager();
	using ResourceTuple = std::tuple<Args*...>;
	std::vector<ResourceTuple> resources_;
	
};

template<class ...Args>
inline RenderResourceManager<Args...>::RenderResourceManager()
{
	static constexpr bool allAreBaseOfRenderResource =
		(std::is_base_of_v<RenderResource, Args> && ...);

	static_assert(allAreBaseOfRenderResource
		&& "RenderResourceクラスを継承していないクラスはインスタンスできません。");

	/*std::function<void(RenderResource** _resource)> func;
	func = [](RenderResource* _resource)
		{
			_resource->
		}

	std::apply([this](RenderResource* resource)
	{
		
	},resources_)*/
}

