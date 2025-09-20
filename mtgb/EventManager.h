#pragma once
#include <vector>
#include <functional>
#include <typeindex>
#include "ISystem.h"
namespace mtgb
{
	class EventBase
	{
	public:
		virtual ~EventBase() = default;
	};
	template<typename EventDataType>
	class Event : public EventBase
	{
	public:
		using EventHandler = std::function<void(const EventDataType&)>;

		void Subscribe(EventHandler _handler)
		{
			handlers_.push_back(_handler);
		}
		void UnSubscribe(EventHandler _handler)
		{
			handlers_.clear();
		}
		void Invoke(const EventDataType& _data)
		{
			for (EventHandler& handler : handlers_)
			{
				handler(_data);
			}
		}
	private:
		std::vector<EventHandler> handlers_;
	};

	class EventManager : public ISystem
	{
	public:
		void Initialize() override {}
		void Update() override {}

		template<typename EventDataType>
		Event<EventDataType>& GetEvent()
		{
			std::type_index typeIdx = std::type_index(typeid(EventDataType));

			auto it = events_.find(typeIdx);
			if (it == events_.end())
			{
				// 存在しない場合は新規作成
				Event<EventDataType>* newEvent = new Event<EventDataType>();
				events_[typeIdx] = newEvent;
				return  static_cast<Event<EventDataType>&>(*events_[typeIdx]);
			}

			// 既存のイベントを返す
			return static_cast<Event<EventDataType>&>(*it->second);
		}

	private:
		std::unordered_map<std::type_index, EventBase*> events_;
	};
}