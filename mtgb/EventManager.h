#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <queue>
#include "ISystem.h"
namespace mtgb
{
	class EventBase
	{
	public:
		virtual ~EventBase() = default;
		virtual void UnsubscribeAll() = 0;
	};

	using EventHandlerId = std::size_t;
	/// <summary>
	/// <para>イベント用のテンプレートクラス。</para>
	/// <para>任意のイベントデータ型に対してハンドラの登録、解除、一括解除、呼び出しを行う。</para>
	/// <para>ハンドラは ID で管理し、Invoke 実行時に保留中の解除要求を処理する。</para>
	/// </summary>
	template<typename EventDataType>
	class Event : public EventBase
	{
	public:
		using EventHandler = std::function<void(const EventDataType&)>;

		/// <summary>
		/// ハンドラを登録し、登録に割り当てられた ID を返す
		/// </summary>
		EventHandlerId Subscribe(EventHandler _handler)
		{
			const EventHandlerId id = nextId++;
			handlers_.emplace(id, std::move(_handler));
			return id;
		}
		
		/// <summary>
		/// 指定したハンドラ ID を保留解除キューに追加する
		/// <para>実際の解除は次回の Invoke 呼び出し時に行う</para>
		/// </summary>
		void Unsubscribe(EventHandlerId _id)
		{
			pendingUnsubscribeIds.push(_id);
		}

		/// <summary>
		/// 登録されているすべてのハンドラを即座に解除する
		/// </summary>
		void UnsubscribeAll() override
		{
			handlers_.clear();
		}

		/// <summary>
		/// 保留中の解除要求を処理し、その後すべてのハンドラに対してイベントデータを渡して呼び出す
		/// </summary>
		void Invoke(const EventDataType& _data)
		{
			while (!pendingUnsubscribeIds.empty())
			{
				handlers_.erase(pendingUnsubscribeIds.front());
				pendingUnsubscribeIds.pop();
			}
			for (auto& handler : handlers_)
			{
				if (handler.second)
				{
					handler.second(_data);
				}
			}
		}
	private:
		std::unordered_map<EventHandlerId,EventHandler> handlers_;
		std::queue<EventHandlerId> pendingUnsubscribeIds;
		EventHandlerId nextId = 1;
	};

	class EventManager : public ISystem
	{
	public:
		void Initialize() override;
		void Update() override {}

		template<typename EventDataType>
		/// <summary>
		/// 指定されたイベントデータ型に対応するEvent オブジェクトを返す
		/// </summary>
		Event<EventDataType>& GetEvent()
		{
			// テンプレート型 `EventDataType` を識別するための type_index を作成
			std::type_index typeIdx = std::type_index(typeid(EventDataType));

			auto it = events_.find(typeIdx);
			if (it == events_.end())
			{
				// 存在しない場合は新規作成してマップに登録
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