#pragma once
#include <vector>
#include "cmtgb.h"
#include "IComponentPool.h"
#include "ISystem.h"

namespace mtgb
{
	static constexpr size_t COMPONENT_CAPACITY{ 1024 };

	//class GameObject;
	class Entity;

	/// <summary>
	/// �R���|�[�l���g��A�������z�u�ɂ���v�[��
	/// </summary>
	/// <typeparam name="ComponentT">�R���|�[�l���g�̌^</typeparam>
	/// <typeparam name="IsSingleton">�P�����i�[�ł��Ȃ��� true / false</typeparam>
	template<class ComponentT, bool IsSingleton = true>
	class ComponentPool : public IComponentPool, public ISystem
	{
	public:
		ComponentPool();
		virtual ~ComponentPool();

		void Initialize() override;
		virtual void Start() {};
		virtual void Update() override {};

		void Release() override;

		/// <summary>
		/// �R���|�[�l���g���쐬/�擾����
		/// </summary>
		/// <param name="_entityId">�G���e�B�e�BId</param>
		/// <returns>�R���|�[�l���g�̎Q�ƃ|�C���^ (�m���ɑ��݂���)</returns>
		ComponentT& Get(EntityId _entityId);

		bool TryGet(ComponentT*& _pComponent, EntityId _entityId);

		ComponentT& Add(EntityId _entityId) requires(!IsSingleton);

		/// <summary>
		/// �G���e�B�e�B�������Ă���R���|�[�l���g���폜����
		/// </summary>
		/// <param name="_entityId">�G���e�B�e�BId</param>
		void Remove(const EntityId _entityId) override;

		/// <summary>
		/// �G���e�B�e�B�ɓo�^���ꂽ�R���|�[�l���g����������
		/// </summary>
		/// <param name="_pEntity">�o�^����Ă���G���e�B�e�B</param>
		void UnRegister(EntityId _entityId);

	protected:
		std::vector<ComponentT> pool_;  // �R���|�[�l���g���̂��̂��i�[����v�[��
		std::vector<EntityId> poolId_;  // �R���|�[�l���g�̓o�^�G���e�B�e�BId
	};

	template<class ComponentT, bool IsSingleton>
	inline ComponentPool<ComponentT, IsSingleton>::ComponentPool() :
		pool_{},
		poolId_{}
	{
		pool_.reserve(COMPONENT_CAPACITY);
		poolId_.reserve(COMPONENT_CAPACITY);
	}

	template<class ComponentT, bool IsSingleton>
	inline ComponentPool<ComponentT, IsSingleton>::~ComponentPool()
	{
	}

	template<class ComponentT, bool IsSingleton>
	inline void ComponentPool<ComponentT, IsSingleton>::Initialize()
	{
		RegisterCurrentScene([&, this] { Release(); });
		Start();
	}

	template<class ComponentT, bool IsSingleton>
	inline void ComponentPool<ComponentT, IsSingleton>::Release()
	{
		pool_.clear();
		poolId_.clear();

		pool_.reserve(COMPONENT_CAPACITY);
		poolId_.reserve(COMPONENT_CAPACITY);
	}

	template<class ComponentT, bool IsSingleton>
	inline ComponentT& ComponentPool<ComponentT, IsSingleton>::Get(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				return pool_[i];  // Id����v�����Y���̃R���|�[�l���g��Ԃ�
			}
		}

		// �v�[���ɑ��݂��Ȃ��Ȃ�V���ɒǉ�
		poolId_.push_back(_entityId);
		// NOTE: emplace_back�Ŏ��̂����̂܂ܒǉ�
		pool_.emplace_back(_entityId);
		// �ǉ������珉��������
		pool_[pool_.size() - 1].Initialize();

		return pool_[pool_.size() - 1];  // �ǉ�&&�����������R���|�[�l���g��Ԃ�
	}

	template<class ComponentT, bool IsSingleton>
	inline bool ComponentPool<ComponentT, IsSingleton>::TryGet(ComponentT*& _pComponent, EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				_pComponent = &pool_[i];  // Id����v�����Y���̃R���|�[�l���g��Ԃ�
				return true;
			}
		}

		return false;
	}

	template<class ComponentT, bool IsSingleton>
	inline ComponentT& ComponentPool<ComponentT, IsSingleton>::Add(EntityId _entityId) requires(!IsSingleton)
	{
		// �v�[���ɑ��݂��Ȃ��Ȃ�V���ɒǉ�
		poolId_.push_back(_entityId);
		// NOTE: emplace_back�Ŏ��̂����̂܂ܒǉ�
		pool_.emplace_back(_entityId);
		// �ǉ������珉��������
		pool_[pool_.size() - 1].Initialize();

		return pool_[pool_.size() - 1];  // �ǉ�&&�����������R���|�[�l���g��Ԃ�
	}

	template<class ComponentT, bool IsSingleton>
	inline void ComponentPool<ComponentT, IsSingleton>::Remove(const EntityId _entityId)
	{
		if constexpr (IsSingleton)
		{
			for (int i = 0; i < poolId_.size(); i++)
			{
				if (poolId_[i] == _entityId)
				{
					poolId_[i] = INVALD_ENTITY;
					return;  // ���������Ȃ疳��Id�ɂ��ĉ�A
				}
			}
		}
		else  // ��������\�����l������
		{
			for (int i = 0; i < poolId_.size(); i++)
			{
				if (poolId_[i] == _entityId)
				{
					poolId_[i] = INVALD_ENTITY;
				}
			}
		}
	}

	template<class ComponentT, bool IsSingleton>
	inline void ComponentPool<ComponentT, IsSingleton>::UnRegister(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				poolId_.erase(poolId_.begin() + i);
				pool_.erase(pool_.begin() + i);
				return;  // �|�C���^����v�����Ȃ����
			}
		}
	}
}
