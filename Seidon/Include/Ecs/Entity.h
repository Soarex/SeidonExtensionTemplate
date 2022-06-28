#pragma once
#include "../Core/UUID.h"

#include "Scene.h"
#include "Components.h"
#include "entt/entt.hpp"

#include <iostream>

namespace Seidon
{
	class Entity
	{
	public:
		entt::entity ID;
		Scene* scene;
		
		//entt::registry* registry;
	public:
		Entity() = default;
		Entity(const Entity& entity) = default;
		Entity(entt::entity id, Scene* scene);

		void Save(std::ofstream& out);

		void Load(std::ifstream& in);

		Entity Duplicate();

		const std::string& GetName();
		const void SetName(const std::string& name);
		UUID GetId();
		glm::mat4 GetLocalTransformMatrix();
		glm::mat4 GetGlobalTransformMatrix();

		bool HasParent();
		Entity GetParent();
		void SetParent(Entity parent);
		void RemoveParent();

		std::vector<UUID>& GetChildrenIds();
		void AddChild(Entity e);
		void RemoveChild(Entity e);

		bool IsAncestorOf(Entity e);
		bool IsDescendantOf(Entity e);

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			T& component = scene->GetRegistry().emplace_or_replace<T>(ID, std::forward<Args>(args)...);
			scene->OnComponentAdded<T>(ID);
			return component;
		}

		template <typename T>
		void EditComponent(std::function<void(T&)> editFunction)
		{
			scene->GetRegistry().patch<T>(ID, editFunction);
		}

		template <typename T>
		void RemoveComponent()
		{
			scene->OnComponentRemoved<T>(ID);
			scene->GetRegistry().remove<T>(ID);
		}

		template <typename T>
		T& GetComponent()
		{
			return scene->GetRegistry().get<T>(ID);
		}

		template <typename T>
		bool HasComponent()
		{
			return scene->GetRegistry().all_of<T>(ID);
		}

		bool IsValid()
		{
			return scene->GetRegistry().valid(ID);
		}

		bool operator ==(const Entity& other) { return ID == other.ID; }
	};
}