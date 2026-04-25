#include <Collision/CollisionSystem.h>
#include <Registry/Registry.h>
#include <Collision/BoxCollider3DComponent.h>
#include <Collision/ColliderTriggerComponent.h>
#include <Collision/ColliderStaticPhysicsComponent.h>
#include <Transform/TransformComponent.h>
#include <Collision/TriggerEvent.h>

using namespace Umi;

CollisionSystem::CollisionSystem(Registry& registry) : registry(registry) {}

CellKey CollisionSystem::GetCellKey(const Vector3& position)
{
	return CellKey{
		static_cast<int>(std::floor(position.x / cellSize)),
		static_cast<int>(std::floor(position.y / cellSize)),
		static_cast<int>(std::floor(position.z / cellSize))
	};
}

void CollisionSystem::AddEntityToGrid(Entity entity, const CellKey& key)
{
	grid[key].push_back(entity);
	entityCellMap[entity] = key;
}

void CollisionSystem::RemoveEntityFromGrid(Entity entity, const CellKey& key)
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

void CollisionSystem::UpdateEntityInGrid(Entity entity, const CellKey& oldKey, const CellKey& newKey)
{
	if (!(oldKey == newKey))
	{
		RemoveEntityFromGrid(entity, oldKey);
		AddEntityToGrid(entity, newKey);
	}
}

std::vector<Entity> CollisionSystem::GetNearbyEntities(const CellKey& key)
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

bool CollisionSystem::CheckAABBCollision(const BoxCollider3DComponent& a, const BoxCollider3DComponent& b)
{
	Vector3 half1 = a.size * 0.5f;
	Vector3 half2 = b.size * 0.5f;

	Vector3 min1 = a.pos - half1;
	Vector3 max1 = a.pos + half1;

	Vector3 min2 = b.pos - half2;
	Vector3 max2 = b.pos + half2;

	return (min1.x < max2.x &&
		max1.x > min2.x &&
		min1.y < max2.y &&
		max1.y > min2.y &&
		min1.z < max2.z &&
		max1.z > min2.z);
}

void CollisionSystem::HandleCollisionPhysics(Transform3DComponent& transform1, BoxCollider3DComponent& collider1, Transform3DComponent& transform2, BoxCollider3DComponent& collider2)
{
	Vector3 center1 = collider1.pos;
	Vector3 center2 = collider2.pos;

	Vector3 half1 = collider1.size * 0.5f;
	Vector3 half2 = collider2.size * 0.5f;

	Vector3 delta = center2 - center1;

	float overlapX = (half1.x + half2.x) - std::abs(delta.x);
	float overlapY = (half1.y + half2.y) - std::abs(delta.y);
	float overlapZ = (half1.z + half2.z) - std::abs(delta.z);

	if (overlapX <= 0 || overlapY <= 0 || overlapZ <= 0)
		return;

	float px = 0, py = 0, pz = 0;

	if (overlapX < overlapY && overlapX < overlapZ)
	{
		px = (delta.x > 0) ? overlapX : -overlapX;
	}
	else if (overlapY < overlapX && overlapY < overlapZ)
	{
		py = (delta.y > 0) ? overlapY : -overlapY;
	}
	else
	{
		pz = (delta.z > 0) ? overlapZ : -overlapZ;
	}

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
		
		collider2.pos.x += px;
		collider2.pos.y += py;
		collider2.pos.z += pz;
	}
	else if (!obj1Static && obj2Static)
	{
		transform1.pos.x -= px;
		transform1.pos.y -= py;
		transform1.pos.z -= pz;
		
		collider1.pos.x -= px;
		collider1.pos.y -= py;
		collider1.pos.z -= pz;
	}
	else
	{
		transform1.pos.x -= px * 0.5f;
		transform1.pos.y -= py * 0.5f;
		transform1.pos.z -= pz * 0.5f;

		transform2.pos.x += px * 0.5f;
		transform2.pos.y += py * 0.5f;
		transform2.pos.z += pz * 0.5f;

		collider1.pos.x -= px * 0.5f;
		collider1.pos.y -= py * 0.5f;
		collider1.pos.z -= pz * 0.5f;

		collider2.pos.x += px * 0.5f;
		collider2.pos.y += py * 0.5f;
		collider2.pos.z += pz * 0.5f;
	}
}

void CollisionSystem::Update()
{
	for (auto e : registry.View<Transform3DComponent, BoxCollider3DComponent>())
	{
		auto& collider = registry.GetComponent<BoxCollider3DComponent>(e);
		auto& transform = registry.GetComponent<Transform3DComponent>(e);

		if (collider.isStatic)
			continue;

		collider.pos = transform.pos;

		CellKey newKey = GetCellKey(collider.pos);
		auto it = entityCellMap.find(e);

		if (it != entityCellMap.end())
		{
			UpdateEntityInGrid(e, it->second, newKey);
		}
		else
		{
			AddEntityToGrid(e, newKey);
		}
	}

	for (auto e : registry.View<BoxCollider3DComponent>())
	{
		auto& ColliderA = registry.GetComponent<BoxCollider3DComponent>(e);
		ColliderA.prevHit = ColliderA.currentHit;
		ColliderA.currentHit = false;

		CellKey currentCell = GetCellKey(ColliderA.pos);
		std::vector<Entity> nearbyEntities = GetNearbyEntities(currentCell);

		for (auto otherEntity : nearbyEntities)
		{
			if (otherEntity == e)
				continue;

			if (registry.HasComponent<Transform3DComponent>(otherEntity) && registry.HasComponent<BoxCollider3DComponent>(otherEntity))
			{
				auto& ColliderB = registry.GetComponent<BoxCollider3DComponent>(otherEntity);
	
				if (ColliderA.isTrigger || ColliderB.isTrigger)
				{
					if (CheckAABBCollision(ColliderA, ColliderB))
					{
						ColliderA.currentHit = true;
						ColliderB.currentHit = true;
						registry.AddComponent<TriggerEvent>(registry.CreateEntity(), TriggerEvent{ e, otherEntity });
					}
					continue;
				}

				if (!ColliderB.isPhysics)
					continue;

				if (CheckAABBCollision(ColliderA, ColliderB))
				{
					ColliderA.currentHit = true;
					ColliderB.currentHit = true;

					HandleCollisionPhysics(
						registry.GetComponent<Transform3DComponent>(e),
						ColliderA,
						registry.GetComponent<Transform3DComponent>(otherEntity),
						ColliderB
					);
				}
			}
		}
	}
}