module;
#include <EngineApi/EngineApi.h>
#include <unordered_map>
export module CollisionManager;

import Entity;
import Registry;
import Math;
import Transform;
import ColliderBox;

export namespace Umi
{
	struct ENGINE_API CellKey
	{
		int x, y, z;

		bool operator==(const CellKey& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}
	};

	struct ENGINE_API CellKeyHash
	{
		std::size_t operator()(const CellKey& key) const
		{
			return std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1) ^ (std::hash<int>()(key.z) << 2);
		}
	};

	class ENGINE_API CollisionManager
	{
	private:
		Registry& registry;

		static constexpr int cellSize = 20;
		std::unordered_map<CellKey, std::vector<Entity>, CellKeyHash> grid;
		std::unordered_map<Entity, CellKey> entityCellMap;

		CellKey GetCellKey(const Vector3& position);
		void AddEntityToGrid(Entity entity, const CellKey& key);
		void RemoveEntityFromGrid(Entity entity, const CellKey& key);
		void UpdateEntityInGrid(Entity entity, const CellKey& oldKey, const CellKey& newKey);
		std::vector<Entity> GetNearbyEntities(const CellKey& key);

		bool CheckAABBCollision(const ColliderBox& a, const ColliderBox& b);
		void HandleCollisionPhysics(Transform& transform1, ColliderBox& collider1, Transform& transform2, ColliderBox& collider2);

	public:
		std::vector<Entity> OverlapBox(const Vector3& center, const Vector3& size);

		void UpdateColliders();
		void Update();

		CollisionManager(Registry& registry) : registry(registry) {}
	};
}