#pragma once
#include "UUID.h"
#include "Asset.h"

#include <unordered_map>
#include <utility>

namespace Seidon
{
	class ResourceManager
	{
	public:
		void Init();
		void Destroy();

		void Save(std::ofstream& out);
		void Load(std::ifstream& in);

		template<typename T>
		T* LoadAsset(const std::string& path, UUID id = UUID())
		{
			T* asset = new T();
			asset->id = id;
			asset->Load(path);

			AddAsset(path, asset);

			idToAssetPath[asset->id] = path;
			assetPathToId[path] = asset->id;
			return asset;
		}

		template<typename T>
		T* LoadAsset(UUID id)
		{
			std::string& path = idToAssetPath[id];

			T* asset = new T();
			asset->id = id;
			asset->Load(path);

			AddAsset(path, asset);

			return asset;
		}

		template<typename T>
		T* GetOrLoadAsset(const std::string& name)
		{
			if (nameToAssetId.count(name) > 0) return (T*)assets[nameToAssetId[name]];

			return LoadAsset<T>(name);
		}

		template<typename T>
		T* GetOrLoadAsset(UUID id)
		{
			if (assets.count(id) > 0) return (T*)assets[id];

			return LoadAsset<T>(id);
		}

		template<typename T>
		T* GetAsset(UUID id) { return (T*)assets[id]; }

		template<typename T>
		T* GetAsset(const std::string& name) { return (T*)assets[nameToAssetId.at(name)]; }

		void AddAsset(const std::string& name, Asset* asset);
		inline void RegisterAsset(Asset* asset, const std::string& path) { idToAssetPath[asset->id] = path; assetPathToId[path] = asset->id; }
		inline void RegisterAssetId(UUID id, const std::string& path) { idToAssetPath[id] = path; assetPathToId[path] = id; }

		inline bool IsAssetRegistered(UUID id) { return idToAssetPath.count(id) > 0; }
		inline bool IsAssetRegistered(const std::string& path) { return assetPathToId.count(path) > 0; }

		inline bool IsAssetLoaded(UUID id) { return assets.count(id) > 0; }
		inline bool IsAssetLoaded(const std::string& name) { return nameToAssetId.count(name) > 0; }

		inline std::string GetAssetPath(UUID id) { return idToAssetPath[id]; }
		inline UUID GetAssetId(const std::string& path) { return assetPathToId[path]; }

		std::vector<Asset*>	GetAssets();

	private:

		std::unordered_map<UUID, Asset*> assets;
		std::unordered_map<std::string, UUID> nameToAssetId;
		std::unordered_map<UUID, std::string> idToAssetPath;
		std::unordered_map<std::string, UUID> assetPathToId;
	};
}