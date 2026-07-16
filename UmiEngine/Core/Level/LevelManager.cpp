module;
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
module LevelManager;

import Entity;
import Transform;
import Texture;
import Model;
import ID;
import Camera;
import ColliderBox;

import Script;

import Error;

using namespace Umi;

namespace
{
    constexpr uint32_t LevelMagic = 0x4C564D55;
    constexpr uint32_t LevelVersion = 1;

    enum class ComponentFlag : uint8_t
    {
        ID = 1 << 0,
        Transform = 1 << 1,
        Texture = 1 << 2,
        Model = 1 << 3,
        Camera = 1 << 4,
		ColliderBox = 1 << 5,
		Script = 1 << 6
    };

	void WriteInt(std::ofstream& file, const int& v)
	{
		file.write(reinterpret_cast<const char*>(&v), sizeof(int));
	}

	int ReadInt(std::ifstream& file)
	{
		int v{};
		file.read(reinterpret_cast<char*>(&v), sizeof(int));
		return v;
	}

    void WriteString(std::ofstream& file, const std::string& str)
    {
        uint32_t len = static_cast<uint32_t>(str.size());
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(str.data(), len);
    }

    std::string ReadString(std::ifstream& file)
    {
        uint32_t len = 0;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string str(len, '\0');
        file.read(str.data(), len);
        return str;
    }

    void WriteVector3(std::ofstream& file, const Vector3& v)
    {
        file.write(reinterpret_cast<const char*>(&v), sizeof(Vector3));
    }

    Vector3 ReadVector3(std::ifstream& file)
    {
        Vector3 v{};
        file.read(reinterpret_cast<char*>(&v), sizeof(Vector3));
        return v;
    }

    void WriteFloat(std::ofstream& file, const float& v)
    {
        file.write(reinterpret_cast<const char*>(&v), sizeof(float));
    }

    float ReadFloat(std::ifstream& file)
    {
        float v{};
        file.read(reinterpret_cast<char*>(&v), sizeof(float));
        return v;
    }
}

bool LevelManager::SaveLevel()
{
    std::ofstream file(currentLevel.levelPath, std::ios::binary);
    if (!file.is_open())
    {
        Error::NonFatalError("Failed to open level file for writing.");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&LevelMagic), sizeof(LevelMagic));
    file.write(reinterpret_cast<const char*>(&LevelVersion), sizeof(LevelVersion));

    std::vector<Entity> entities;
    for (Entity e = static_cast<Entity>(0); e < registry.maxEntity; e++)
    {
        if (registry.HasComponent<ID>(e) || registry.HasComponent<Transform>(e) ||
            registry.HasComponent<Texture>(e) || registry.HasComponent<Model>(e) || registry.HasComponent<Camera>(e) || registry.HasComponent<ColliderBox>(e) || registry.HasComponent<Scripts>(e))
        {
            entities.push_back(e);
        }
    }

    uint32_t entityCount = static_cast<uint32_t>(entities.size());
    file.write(reinterpret_cast<const char*>(&entityCount), sizeof(entityCount));

    for (Entity e : entities)
    {
        uint8_t flags = 0;
        if (registry.HasComponent<ID>(e)) flags |= static_cast<uint8_t>(ComponentFlag::ID);
        if (registry.HasComponent<Transform>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Transform);
        if (registry.HasComponent<Texture>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Texture);
        if (registry.HasComponent<Model>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Model);
        if (registry.HasComponent<Camera>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Camera);
		if (registry.HasComponent<ColliderBox>(e)) flags |= static_cast<uint8_t>(ComponentFlag::ColliderBox);
        if (registry.HasComponent<Scripts>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Script);

        file.write(reinterpret_cast<const char*>(&flags), sizeof(flags));

        if (flags & static_cast<uint8_t>(ComponentFlag::ID))
        {
            WriteString(file, registry.GetComponent<ID>(e).name);
            WriteString(file, registry.GetComponent<ID>(e).tag);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::Transform))
        {
            Transform t = registry.GetComponent<Transform>(e);
            WriteVector3(file, t.pos);
            WriteVector3(file, t.rot);
            WriteVector3(file, t.scale);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::Texture))
        {
            Texture t = registry.GetComponent<Texture>(e);
            WriteString(file, textureManager.GetTextureData(t.id).filePath);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::Model))
        {
            Model t = registry.GetComponent<Model>(e);
            WriteString(file, modelManager.GetModelData(t.id).filePath);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::Camera))
        {
            Camera t = registry.GetComponent<Camera>(e);
            WriteFloat(file, t.fov);
            WriteFloat(file, t.nearClip);
            WriteFloat(file, t.farClip);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::ColliderBox))
        {
            ColliderBox t = registry.GetComponent<ColliderBox>(e);
            WriteVector3(file, t.localPosition);
            WriteVector3(file, t.localRotation);
            WriteVector3(file, t.localScale);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::Script))
        {
            WriteInt(file, registry.GetComponent<Scripts>(e).scripts.size());
            for (const auto& t : registry.GetComponent<Scripts>(e).scripts)
            {
                WriteString(file, t.name);
            }
        }
    }

    return true;
}

bool LevelManager::SaveLevel(const std::string& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        Error::NonFatalError("Failed to open level file for writing.");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&LevelMagic), sizeof(LevelMagic));
    file.write(reinterpret_cast<const char*>(&LevelVersion), sizeof(LevelVersion));

    // Collect valid entities first
    std::vector<Entity> entities;
    for (Entity e = static_cast<Entity>(0); e < registry.maxEntity; e++)
    {
        if (registry.HasComponent<ID>(e) || registry.HasComponent<Transform>(e) ||
            registry.HasComponent<Texture>(e) || registry.HasComponent<Model>(e) || registry.HasComponent<Camera>(e) || registry.HasComponent<ColliderBox>(e) || registry.HasComponent<Scripts>(e))
        {
            entities.push_back(e);
        }
    }

    uint32_t entityCount = static_cast<uint32_t>(entities.size());
    file.write(reinterpret_cast<const char*>(&entityCount), sizeof(entityCount));

    for (Entity e : entities)
    {
        uint8_t flags = 0;
        if (registry.HasComponent<ID>(e)) flags |= static_cast<uint8_t>(ComponentFlag::ID);
        if (registry.HasComponent<Transform>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Transform);
        if (registry.HasComponent<Texture>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Texture);
        if (registry.HasComponent<Model>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Model);
		if (registry.HasComponent<Camera>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Camera);
		if (registry.HasComponent<ColliderBox>(e)) flags |= static_cast<uint8_t>(ComponentFlag::ColliderBox);
		if (registry.HasComponent<Scripts>(e)) flags |= static_cast<uint8_t>(ComponentFlag::Script);

        file.write(reinterpret_cast<const char*>(&flags), sizeof(flags));

        if (flags & static_cast<uint8_t>(ComponentFlag::ID))
        {
            WriteString(file, registry.GetComponent<ID>(e).name);
			WriteString(file, registry.GetComponent<ID>(e).tag);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Transform))
        {
            Transform t = registry.GetComponent<Transform>(e);
            WriteVector3(file, t.pos);
            WriteVector3(file, t.rot);
            WriteVector3(file, t.scale);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Texture))
        {
            Texture t = registry.GetComponent<Texture>(e);
            WriteString(file, textureManager.GetTextureData(t.id).filePath);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Model))
        {
            Model t = registry.GetComponent<Model>(e);
            WriteString(file, modelManager.GetModelData(t.id).filePath);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Camera))
        {
            Camera t = registry.GetComponent<Camera>(e);
            WriteFloat(file, t.fov);
            WriteFloat(file, t.nearClip);
            WriteFloat(file, t.farClip);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::ColliderBox))
        {
            ColliderBox t = registry.GetComponent<ColliderBox>(e);
            WriteVector3(file, t.localPosition);
            WriteVector3(file, t.localRotation);
            WriteVector3(file, t.localScale);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Script))
        {
            WriteInt(file, registry.GetComponent<Scripts>(e).scripts.size());
            for (const auto& t : registry.GetComponent<Scripts>(e).scripts)
            {
                WriteString(file, t.name);
            }
        }   
    }

    return true;
}

bool LevelManager::LoadLevel(const std::string& path)
{
    registry.Clear();

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        Error::NonFatalError("Failed to open level file for reading.");
        return false;
    }

    uint32_t magic = 0;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != LevelMagic)
    {
        Error::NonFatalError("Invalid level file format.");
        return false;
    }

    uint32_t version = 0;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != LevelVersion)
    {
        Error::NonFatalError("Unsupported level file version.");
        return false;
    }

    currentLevel.levelPath = path;
    // currentLevel.levelName = path.substr(path.find_last_of('\\') + 1);

    uint32_t entityCount = 0;
    file.read(reinterpret_cast<char*>(&entityCount), sizeof(entityCount));

    for (uint32_t i = 0; i < entityCount; i++)
    {
        uint8_t flags = 0;
        file.read(reinterpret_cast<char*>(&flags), sizeof(flags));

        Entity e = registry.CreateEntity();

        if (flags & static_cast<uint8_t>(ComponentFlag::ID))
        {
            ID id;
            id.name = ReadString(file);
			id.tag = ReadString(file);
            registry.AddComponent<ID>(e, id);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Transform))
        {
            Transform transform;
            transform.pos = ReadVector3(file);
            transform.rot = ReadVector3(file);
            transform.scale = ReadVector3(file);
            registry.AddComponent<Transform>(e, transform);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Texture))
        {
            Texture texture;
            texture.id = textureManager.LoadTexture2D(ReadString(file));
            registry.AddComponent<Texture>(e, texture);
        }
        if (flags & static_cast<uint8_t>(ComponentFlag::Model))
        {
            Model model;
            model.id = modelManager.LoadModel(ReadString(file));
            registry.AddComponent<Model>(e, model);
        }

        if (flags & static_cast<uint8_t>(ComponentFlag::Camera))
        {
            Camera camera;
            camera.fov = ReadFloat(file);
            camera.nearClip = ReadFloat(file);
            camera.farClip = ReadFloat(file);
            registry.AddComponent<Camera>(e, camera);
        }
		if (flags & static_cast<uint8_t>(ComponentFlag::ColliderBox))
		{
			ColliderBox collider;
			collider.localPosition = ReadVector3(file);
			collider.localRotation = ReadVector3(file);
			collider.localScale = ReadVector3(file);
			registry.AddComponent<ColliderBox>(e, collider);
		}
		if (flags & static_cast<uint8_t>(ComponentFlag::Script))
		{
			int scriptCount = ReadInt(file);
			for (int j = 0; j < scriptCount; j++)
			{
                std::string scriptName = ReadString(file);
                scriptManager.AddScriptToEntity(e, scriptName);
			}
		}
    }

    return true;
}

bool LevelManager::CreateLevel(const std::string& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        Error::NonFatalError("Failed to open level file for writing.");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&LevelMagic), sizeof(LevelMagic));
    file.write(reinterpret_cast<const char*>(&LevelVersion), sizeof(LevelVersion));

    uint32_t entityCount = 0;
    file.write(reinterpret_cast<const char*>(&entityCount), sizeof(entityCount));

    uint8_t flags = 0;
    file.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
}