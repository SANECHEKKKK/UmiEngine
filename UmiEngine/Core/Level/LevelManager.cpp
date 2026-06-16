module;
#include <Json/json.hpp>
#include <fstream>
module LevelManager;

import Entity;
import Transform;
import Texture;
import Model;
import ID;
import Error;

using json = nlohmann::json;
using namespace Umi;

bool LevelManager::SaveLevel()
{
    json levelJson;
    std::vector<json> entities;
    auto& registry = engineContext.registry;

    for (Entity e = static_cast<Entity>(1); e < registry.maxEntity; e++)
    {
        json entityJson;
        
        if (registry.HasComponent<ID>(e))
        {
            ID t = registry.GetComponent<ID>(e);
            
            entityJson["ID"] = t.name;
        }
        
        if (registry.HasComponent<Transform>(e))
        {
            Transform t = registry.GetComponent<Transform>(e);

            entityJson["Transform"]["pos"]["x"] = t.pos.x;
            entityJson["Transform"]["pos"]["y"] = t.pos.y;
            entityJson["Transform"]["pos"]["z"] = t.pos.z;
            entityJson["Transform"]["rot"]["x"] = t.rot.x;
            entityJson["Transform"]["rot"]["y"] = t.rot.y;
            entityJson["Transform"]["rot"]["z"] = t.rot.z;
            entityJson["Transform"]["scale"]["x"] = t.scale.x;
            entityJson["Transform"]["scale"]["y"] = t.scale.y;
            entityJson["Transform"]["scale"]["z"] = t.scale.z;
        }

        if (registry.HasComponent<Texture>(e))
        {
            Texture t = registry.GetComponent<Texture>(e);

            entityJson["Texture"] = engineContext.textureManager.GetTextureData(t.id).filePath;
        }

        if (registry.HasComponent<Model>(e))
        {
            Model t = registry.GetComponent<Model>(e);
            entityJson["Model"] = engineContext.modelManager.GetModelData(t.id).filePath;
        }

        entities.emplace_back(entityJson);
    }

    levelJson["Entities"] = entities;

    std::ofstream file("Level.json");
    file << levelJson;

    return true;
}

bool LevelManager::LoadLevel()
{
    auto& registry = engineContext.registry;
    
    // registry.Clear();

    json levelJson;
    std::ifstream file("Level.json");
    if (!file.is_open())
    {
        //handle non fatal error
    }
    file >> levelJson;

    if (levelJson.contains("Entities"))
    {
        for (json entity : levelJson["Entities"])
        {
            Entity e = registry.CreateEntity();

            if (entity.contains("ID"))
            {
                ID id;
                id.name = entity["ID"].get<std::string>();
                
                registry.AddComponent<ID>(e, id);
            }
            
            if (entity.contains("Transform"))
            {
                Transform transform;

                transform.pos = Vector3{
                    entity["Transform"]["pos"]["x"].get<float>(), entity["Transform"]["pos"]["y"].get<float>(),
                    entity["Transform"]["pos"]["z"].get<float>()
                };
                transform.rot = Vector3{
                    entity["Transform"]["rot"]["x"].get<float>(), entity["Transform"]["rot"]["y"].get<float>(),
                    entity["Transform"]["rot"]["z"].get<float>()
                };
                transform.scale = Vector3{
                    entity["Transform"]["scale"]["x"].get<float>(), entity["Transform"]["scale"]["y"].get<float>(),
                    entity["Transform"]["scale"]["z"].get<float>()
                };

                registry.AddComponent<Transform>(e, transform);
            }

            if (entity.contains("Texture"))
            {
                Texture texture;
                texture.id = engineContext.textureManager.LoadTexture2D(entity["Texture"].get<std::string>());

                registry.AddComponent<Texture>(e, texture);
            }

            if (entity.contains("Model"))
            {
                Model model;
                model.id = engineContext.modelManager.LoadModel(entity["Model"].get<std::string>());

                registry.AddComponent<Model>(e, model);
            }
        }
    }

    return true;
}
