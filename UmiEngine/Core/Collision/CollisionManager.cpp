module;
#include <algorithm>
#include <cmath>
#include <unordered_map>
module CollisionManager;

using namespace Umi;

CellKey CollisionManager::GetCellKey(const Vector3& position)
{
	return CellKey{
	static_cast<int>(std::floor(position.x / cellSize)),
	static_cast<int>(std::floor(position.y / cellSize)),
	static_cast<int>(std::floor(position.z / cellSize))
	};
}

void CollisionManager::AddEntityToGrid(Entity entity, const CellKey& key)
{
	grid[key].push_back(entity);
	entityCellMap[entity] = key;

}

void CollisionManager::RemoveEntityFromGrid(Entity entity, const CellKey& key)
{
	auto it = grid.find(key);
	if (it != grid.end())
	{
		auto& entities = it->second;
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());

		if (entities.empty())
		{
			grid.erase(it);
		}
	}
}

void CollisionManager::UpdateEntityInGrid(Entity entity, const CellKey& oldKey, const CellKey& newKey)
{
	if (!(oldKey == newKey))
	{
		RemoveEntityFromGrid(entity, oldKey);
		AddEntityToGrid(entity, newKey);
	}

}

std::vector<Entity> CollisionManager::GetNearbyEntities(const CellKey& key)
{
	std::vector<Entity> nearby;

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				CellKey neighborKey = { key.x + x, key.y + y, key.z + z };
				auto it = grid.find(neighborKey);
				if (it != grid.end())
				{
					nearby.insert(nearby.end(), it->second.begin(), it->second.end());
				}
			}
		}
	}

	return nearby;
}

bool CollisionManager::CheckAABBCollision(const ColliderBox& a, const ColliderBox& b)
{
	Vector3 half1 = a.size * 0.5f;
	Vector3 half2 = b.size * 0.5f;

	Vector3 min1 = a.worldPosition - half1;
	Vector3 max1 = a.worldPosition + half1;

	Vector3 min2 = b.worldPosition - half2;
	Vector3 max2 = b.worldPosition + half2;

	return (min1.x < max2.x &&
		max1.x > min2.x &&
		min1.y < max2.y &&
		max1.y > min2.y &&
		min1.z < max2.z &&
		max1.z > min2.z);
}

void CollisionManager::HandleCollisionPhysics(Transform& transform1, ColliderBox& collider1, Transform& transform2, ColliderBox& collider2)
{
	Vector3 center1 = collider1.worldPosition;
	Vector3 center2 = collider2.worldPosition;

	Vector3 half1 = collider1.size * 0.5f;
	Vector3 half2 = collider2.size * 0.5f;

	Vector3 delta = center2 - center1;

	float overlapX = (half1.x + half2.x) - std::abs(delta.x);
	float overlapY = (half1.y + half2.y) - std::abs(delta.y);
	float overlapZ = (half1.z + half2.z) - std::abs(delta.z);

	if (overlapX <= 0 || overlapY <= 0 || overlapZ <= 0)
		return;

	float px = 0, py = 0, pz = 0;

	if (overlapX <= overlapY && overlapX <= overlapZ)
		px = (delta.x > 0) ? overlapX : -overlapX;
	else if (overlapY <= overlapX && overlapY <= overlapZ)
		py = (delta.y > 0) ? overlapY : -overlapY;
	else
		pz = (delta.z > 0) ? overlapZ : -overlapZ;

	bool obj1Static = collider1.isStatic;
	bool obj2Static = collider2.isStatic;

	if (obj1Static && obj2Static)
	{
		return;
	}
	else if (obj1Static && !obj2Static)
	{
		transform2.pos.x += px;
		transform2.pos.y += py;
		transform2.pos.z += pz;

		collider2.worldPosition.x += px;
		collider2.worldPosition.y += py;
		collider2.worldPosition.z += pz;
	}
	else if (!obj1Static && obj2Static)
	{
		transform1.pos.x -= px;
		transform1.pos.y -= py;
		transform1.pos.z -= pz;

		collider1.worldPosition.x -= px;
		collider1.worldPosition.y -= py;
		collider1.worldPosition.z -= pz;
	}
	else
	{
		transform1.pos.x -= px * 0.5f;
		transform1.pos.y -= py * 0.5f;
		transform1.pos.z -= pz * 0.5f;

		transform2.pos.x += px * 0.5f;
		transform2.pos.y += py * 0.5f;
		transform2.pos.z += pz * 0.5f;

		collider1.worldPosition.x -= px * 0.5f;
		collider1.worldPosition.y -= py * 0.5f;
		collider1.worldPosition.z -= pz * 0.5f;

		collider2.worldPosition.x += px * 0.5f;
		collider2.worldPosition.y += py * 0.5f;
		collider2.worldPosition.z += pz * 0.5f;
	}
}

std::vector<Entity> CollisionManager::OverlapBox(const Vector3& center, const Vector3& size)
{
	std::vector<Entity> hits;

	ColliderBox query;
	query.worldPosition = center;
	query.size = size;

	for (Entity e : GetNearbyEntities(GetCellKey(center)))
	{
		auto& collider = registry.GetComponent<ColliderBox>(e);
		if (CheckAABBCollision(query, collider))
			hits.push_back(e);
	}

	return hits;
}

void CollisionManager::UpdateColliders()
{
	for (auto e : registry.View<Transform, ColliderBox>())
	{
		auto& transform = registry.GetComponent<Transform>(e);
		auto& collider = registry.GetComponent<ColliderBox>(e);

		collider.worldPosition = transform.pos + collider.localPosition;
		collider.worldRotation = transform.rot + collider.localRotation;
	}
}

void CollisionManager::Update()
{
	UpdateColliders();
	
	for (auto& [key, entities] : grid)
		entities.clear();

	for (auto e : registry.View<Transform, ColliderBox>())
	{
		auto& collider = registry.GetComponent<ColliderBox>(e);
		grid[GetCellKey(collider.worldPosition)].push_back(e);
	}

	for (auto e : registry.View<Transform, ColliderBox>())
	{
		auto& colliderA = registry.GetComponent<ColliderBox>(e);
		if (colliderA.isStatic)
			continue;

		CellKey key = GetCellKey(colliderA.worldPosition);

		for (Entity other : GetNearbyEntities(key))
		{
			if (other == e)
				continue;

			auto& colliderB = registry.GetComponent<ColliderBox>(other);

			if (!colliderB.isStatic && other < e)
				continue;

			if (CheckAABBCollision(colliderA, colliderB))
			{
				auto& transformA = registry.GetComponent<Transform>(e);
				auto& transformB = registry.GetComponent<Transform>(other);
				HandleCollisionPhysics(transformA, colliderA, transformB, colliderB);
			}
		}
	}
}
