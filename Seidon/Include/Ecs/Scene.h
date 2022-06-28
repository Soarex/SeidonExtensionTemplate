#pragma once

#include "Core/Asset.h"
#include "EnttWrappers.h"
#include "System.h"

#include <unordered_map>
#include <string>
#include <typeinfo>
#include <iostream>
#include <functional>

#include <entt/entt.hpp>
#include <glm/glm.hpp>


namespace Seidon
{
	class Entity;
	class Prefab;

	typedef std::function<void(EntityId)> ComponentCallback;
	typedef std::vector<ComponentCallback>::iterator ComponentCallbackId;

	class Scene : public Asset
	{
	public:
		Scene(const std::string& name = "Scene", UUID id = UUID());
		~Scene();

		void Init();
		void Update(float deltaTime);
		void Destroy();

		using Asset::Save;
		using Asset::Load;

		void Save(std::ofstream& out);
		void Load(std::ifstream& in);

		Scene* Duplicate();
		void CopyEntities(Scene* other);
		void CopySystems(Scene* other);

		Entity CreateEntity(const std::string& name = std::string(), const UUID& id = UUID());
		Entity InstantiatePrefab(Prefab& prefab, const std::string& name = "");
		Entity InstantiatePrefab(Prefab& prefab, const glm::vec3& position, const glm::vec3& rotation,
			const glm::vec3& scale, const std::string& name = "");

		Entity GetEntityByEntityId(EntityId id);
		Entity GetEntityById(UUID id);

		void DestroyEntity(Entity& entity);

		inline bool IsEntityIdValid(EntityId id) { return registry.valid(id); }
		inline bool IsIdValid(UUID id) { return idToEntityMap.count(id); }
		

		inline void SetName(const std::string& name) { this->name = name; }

		inline entt::registry& GetRegistry() { return registry; }

		inline const std::string& GetName() { return name; }

		template <typename... OwnedTypes, typename... ObservedTypes>
		auto CreateComponentGroup(TypeList<ObservedTypes...> observedTypes = {})
		{
			return registry.group<OwnedTypes...>(observedTypes);
		}

		template <typename Type>
		auto CreateComponentView()
		{
			return registry.view<Type>();
		}

		template <typename Type, typename Func>
		void Iterate(const ComponentView<Type>& view, const Func& function)
		{
			view.each(function);
		}

		template <typename... Types, typename Func>
		void Iterate(const ComponentGroup<Types...>& group, const Func& function)
		{
			group.each(function);
		}

		template <typename... OwnedTypes, typename... ObservedTypes, typename Func>
		void CreateGroupAndIterate(TypeList<ObservedTypes...> observedTypes, const Func& function)
		{
			auto g = registry.group<OwnedTypes...>(observedTypes);

			g.each(function);
		}

		template <typename Type, typename Func>
		void CreateViewAndIterate(const Func& function)
		{
			auto v = registry.view<Type>();

			v.each(function);
		}

		template <typename T, typename... Args>
		T& AddSystem(Args&&... args)
		{
			systems[typeid(T).name()] = new T(std::forward<Args>(args)...);
			systems[typeid(T).name()]->scene = this;
			systems[typeid(T).name()]->SysInit();
			return *((T*)systems[typeid(T).name()]);
		}

		template <typename T>
		T& GetSystem()
		{
			return *((T*)systems[typeid(T).name()]);
		}

		template <typename T>
		bool HasSystem()
		{
			return systems.count(typeid(T).name()) > 0;
		}

		template <typename T>
		void DeleteSystem()
		{
			systems[typeid(T).name()]->SysDestroy();
			delete systems[typeid(T).name()];
			systems.erase(typeid(T).name());
		}

		template <typename T>
		ComponentCallbackId AddComponentAddedCallback(const ComponentCallback& callback)
		{
			componentAddedCallbacks[typeid(T).name()].push_back(callback);
			auto it = componentAddedCallbacks[typeid(T).name()].end();
			return --it;
		}

		template <typename T>
		ComponentCallbackId AddComponentRemovedCallback(const ComponentCallback& callback)
		{
			componentRemovedCallbacks[typeid(T).name()].push_back(callback);
			auto it = componentRemovedCallbacks[typeid(T).name()].end(); 
			return --it;
		}

		template <typename T>
		void RemoveComponentAddedCallback(const ComponentCallbackId& callbackId)
		{
			componentAddedCallbacks[typeid(T).name()].erase(callbackId);
		}

		template <typename T>
		void RemoveComponentRemovedCallback(const ComponentCallbackId& callbackId)
		{
			componentRemovedCallbacks[typeid(T).name()].erase(callbackId);
		}

		private:
			entt::registry registry;

			std::unordered_map<UUID, EntityId> idToEntityMap;

			std::unordered_map<std::string, System*> systems;

			std::unordered_map<std::string, std::vector<ComponentCallback>> componentAddedCallbacks;
			std::unordered_map<std::string, std::vector<ComponentCallback>> componentRemovedCallbacks;

		private:
			template <typename T>
			void OnComponentAdded(EntityId entityId)
			{
				for(auto& callback : componentAddedCallbacks[typeid(T).name()])
					callback(entityId);
			}

			template <typename T>
			void OnComponentRemoved(EntityId entityId)
			{
				for (auto& callback : componentRemovedCallbacks[typeid(T).name()])
					callback(entityId);
			}

			void AddChildEntityFromPrefab(Entity parentEntity, Entity prefabEntity);
			friend class Entity;
	};
}