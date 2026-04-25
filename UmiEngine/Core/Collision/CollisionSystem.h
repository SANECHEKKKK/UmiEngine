#pragma once
#include <Entity/Entity.h>
#include <Math/Math.h>
#include <unordered_map>
#include <vector>

namespace Umi
{
	class Registry;
	struct Transform3DComponent;
	struct BoxCollider3DComponent;

	struct CellKey
	{
		int x, y, z;

		bool operator==(const CellKey& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}
	};

	struct CellKeyHash
	{
		std::size_t operator()(const CellKey& key) const
		{
			return std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1) ^ (std::hash<int>()(key.z) << 2);
		}
	};

	class CollisionSystem
	{
	private:
		Registry& registry;

		const int cellSize = 20;
		std::unordered_map<CellKey, std::vector<Entity>, CellKeyHash> grid;
		std::unordered_map<Entity, CellKey> entityCellMap;

		CellKey GetCellKey(const Vector3& position);
		void AddEntityToGrid(Entity entity, const CellKey& key);
		void RemoveEntityFromGrid(Entity entity, const CellKey& key);
		void UpdateEntityInGrid(Entity entity, const CellKey& oldKey, const CellKey& newKey);
		std::vector<Entity> GetNearbyEntities(const CellKey& key);

		void HandleCollisionPhysics(Transform3DComponent& transform1, BoxCollider3DComponent& collider1, Transform3DComponent& transform2, BoxCollider3DComponent& collider2);
		bool CheckAABBCollision(const BoxCollider3DComponent& a, const BoxCollider3DComponent& b);

	public:
		void Update();

		CollisionSystem(Registry& registry);
	};
}