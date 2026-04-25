#include <Prefab/PrefabManager.h>
#include <Prefab/PrefabRequest.h>
#include <EngineContext/EngineContext.h>
#include <Texture/TextureManager.h>
#include <Model/ModelManager.h>
#include <Registry/Registry.h>

#include <Transform/TransformComponent.h>
#include <Texture/TextureComponent.h>
#include <Ui/UiComponent.h>
#include <Button/ButtonComponent.h>
#include <Layer/LayerComponent.h>
#include <Model/ModelComponent.h>
#include <Animation/Animator3DComponent.h>
#include <Animation/AnimationState.h>
#include <State/StateComponent.h>
#include <Input/InputComponent.h>
//#include <Movement/MovementComponent.h>
#include <Ai/AiComponent.h>
#include <Collision/BoxCollider3DComponent.h>
#include <Bar/BarComponent.h>
#include <DropDownMenu/DropDownMenu.h>
#include <DropDownMenu/DropDownMenuHeader.h>
#include <Text/TextComponent.h>

#include <Events/ResolutionChange/ResolutionChange.h>
#include <Events/LoadState/LoadState.h>
#include <Events/OpenSettings/OpenSettings.h>
#include <Events/QuitGame/QuitGame.h>
#include <Events/ExitToMainMenu/ExitToMainMenu.h>

#include <Tag/TagComponent.h>


#include <Json/json.hpp>
#include <fstream>
using json = nlohmann::json;

using namespace Umi;

Anchor StringToAnchor(const std::string& _anchorname)
{
	if (_anchorname == "TopLeft") return Anchor::TopLeft;
	if (_anchorname == "TopCenter") return Anchor::TopCenter;
	if (_anchorname == "TopRight") return Anchor::TopRight;
	if (_anchorname == "MiddleLeft") return Anchor::MiddleLeft;
	if (_anchorname == "MiddleCenter") return Anchor::MiddleCenter;
	if (_anchorname == "MiddleRight") return Anchor::MiddleRight;
	if (_anchorname == "BottomLeft") return Anchor::BottomLeft;
	if (_anchorname == "BottomCenter") return Anchor::BottomCenter;
	if (_anchorname == "BottomRight") return Anchor::BottomRight;

	// Default case, should not happen  
	return Anchor::TopLeft;
}

std::string PrefabManager::ResolvePrefabPath(const std::string& folder, const std::string& name) const
{
	return "Assets\\Prefabs\\" + folder + "\\" + name + ".json";
}

void PrefabManager::LoadPrefab(const std::string& path, const PrefabName& prefabName)
{
	bool modelStatic = false;

	std::ifstream prefabFile(path);
	if (!prefabFile.is_open())
	{
		throw std::runtime_error("Failed to open prefab file: " + path);
	}

	json prefabJson;
	prefabFile >> prefabJson;
	Prefab prefab;

	if (prefabJson.contains("Transform2DComponent"))
	{
		AddPrefabComponent(prefab, Transform2DComponent(
			prefabJson["Transform2DComponent"]["Pos"]["x"],
			prefabJson["Transform2DComponent"]["Pos"]["y"],
			prefabJson["Transform2DComponent"]["Size"]["x"],
			prefabJson["Transform2DComponent"]["Size"]["y"],
			prefabJson["Transform2DComponent"]["Scale"]["x"],
			prefabJson["Transform2DComponent"]["Scale"]["y"],
			prefabJson["Transform2DComponent"]["Rot"]
		));
	}

	if (prefabJson.contains("Transform3DComponent"))
	{
		AddPrefabComponent(prefab, Transform3DComponent(
			prefabJson["Transform3DComponent"]["Pos"]["x"],
			prefabJson["Transform3DComponent"]["Pos"]["y"],
			prefabJson["Transform3DComponent"]["Pos"]["z"],
			prefabJson["Transform3DComponent"]["Scale"]["x"],
			prefabJson["Transform3DComponent"]["Scale"]["y"],
			prefabJson["Transform3DComponent"]["Scale"]["z"],
			prefabJson["Transform3DComponent"]["Rot"]["x"],
			prefabJson["Transform3DComponent"]["Rot"]["y"],
			prefabJson["Transform3DComponent"]["Rot"]["z"]
		));
	}

	if (prefabJson.contains("TextureComponent"))
	{
		std::string texPath = prefabJson["TextureComponent"]["Path"].get<std::string>();
		std::wstring bufferWString = std::wstring(texPath.begin(), texPath.end());
		if (prefabJson["TextureComponent"].contains("Alpha"))
		{
			TextureComponent textureComp(engineContext.textureManager.LoadTexure(bufferWString));
			textureComp.alpha = prefabJson["TextureComponent"]["Alpha"].get<float>();
			AddPrefabComponent(prefab, textureComp);
		}
		else
		{
			AddPrefabComponent(prefab, TextureComponent(engineContext.textureManager.LoadTexure(bufferWString)));
		}
	}

	if (prefabJson.contains("UiComponent"))
	{
		AddPrefabComponent(prefab, UiComponent(
			StringToAnchor(prefabJson["UiComponent"]["Anchor"].get<std::string>()),
			prefabJson["UiComponent"]["Width"].get<float>(),
			prefabJson["UiComponent"]["Height"].get<float>(),
			prefabJson["UiComponent"]["OffsetX"].get<float>(),
			prefabJson["UiComponent"]["OffsetY"].get<float>()
		));

		AddPrefabComponent(prefab, TransformUiComponent());
	}

	if (prefabJson.contains("ButtonComponent"))
	{
		std::string texPath = prefabJson["ButtonComponent"]["Textures"]["Normal"].get<std::string>();
		std::wstring bufferWString = std::wstring(texPath.begin(), texPath.end());
		TextureID normalTex = engineContext.textureManager.LoadTexure(bufferWString);

		texPath = prefabJson["ButtonComponent"]["Textures"]["Hovered"].get<std::string>();
		bufferWString = std::wstring(texPath.begin(), texPath.end());
		TextureID hoveredTex = engineContext.textureManager.LoadTexure(bufferWString);

		texPath = prefabJson["ButtonComponent"]["Textures"]["Pressed"].get<std::string>();
		bufferWString = std::wstring(texPath.begin(), texPath.end());
		TextureID pressedTex = engineContext.textureManager.LoadTexure(bufferWString);

		texPath = prefabJson["ButtonComponent"]["Textures"]["Disabled"].get<std::string>();
		bufferWString = std::wstring(texPath.begin(), texPath.end());
		TextureID disabledTex = engineContext.textureManager.LoadTexure(bufferWString);

		AddPrefabComponent(prefab, ButtonComponent{
			.state = ButtonState::Normal,
			.textures = {
				normalTex,
				hoveredTex,
				pressedTex,
				disabledTex
			}
			});

		AddPrefabComponent(prefab, TextureComponent(normalTex));
	}

	if (prefabJson.contains("DropDownComponent"))
	{

	}

	if (prefabJson.contains("LayerComponent"))
	{
		AddPrefabComponent(prefab, LayerComponent(
			prefabJson["LayerComponent"].get<int>()
		));
	}

	if (prefabJson.contains("ModelComponent"))
	{
		modelStatic = true;

		std::string modelPath = prefabJson["ModelComponent"].get<std::string>();
		AddPrefabComponent(prefab, ModelComponent(engineContext.modelManager.LoadModel(modelPath)));
	}

	if (prefabJson.contains("Animator3DComponent"))
	{
		modelStatic = false;

		Animator3DComponent animatorComp;

		auto& state = prefabJson["Animator3DComponent"]["States"];
		for (auto it = state.begin(); it != state.end(); it++)
		{
			const std::string& state = it.key();
			const std::string& animName = it.value().get<std::string>();

			animatorComp.animNames[GenerateStringID(state.c_str())] = animName;
		}
		AddPrefabComponent(prefab, animatorComp);
	}

	if (prefabJson.contains("StateComponent"))
	{
		AddPrefabComponent(prefab, StateComponent());
	}

	if (prefabJson.contains("InputComponent"))
	{
		AddPrefabComponent(prefab, InputComponent());
	}

	//if (prefabJson.contains("MovementComponent"))
	//{
	//	AddPrefabComponent(prefab, MovementComponent(
	//		{
	//		.friction = prefabJson["MovementComponent"]["Friction"].get<float>(),
	//		.movementSpeed = prefabJson["MovementComponent"]["MovementSpeed"].get<float>(),
	//		}
	//		));
	//}

	if (prefabJson.contains("AiFollowBehaviorComponent"))
	{
		AddPrefabComponent(prefab, AiFollowBehaviorComponent());
	}

	if (prefabJson.contains("BoxCollider3DComponent"))
	{
		BoxCollider3DComponent buffer = BoxCollider3DComponent(
			{
			prefabJson["BoxCollider3DComponent"]["Pos"]["x"].get<float>(),
			prefabJson["BoxCollider3DComponent"]["Pos"]["y"].get<float>(),
			prefabJson["BoxCollider3DComponent"]["Pos"]["z"].get<float>(),
			},
			{
			prefabJson["BoxCollider3DComponent"]["Size"]["x"].get<float>(),
			prefabJson["BoxCollider3DComponent"]["Size"]["y"].get<float>(),
			prefabJson["BoxCollider3DComponent"]["Size"]["z"].get<float>(),
			}
			);

		if (prefabJson["BoxCollider3DComponent"].contains("IsPhysics"))
		{
			buffer.isPhysics = prefabJson["BoxCollider3DComponent"]["IsPhysics"].get<bool>();
		}
		if (prefabJson["BoxCollider3DComponent"].contains("IsStatic"))
		{
			buffer.isStatic = prefabJson["BoxCollider3DComponent"]["IsStatic"].get<bool>();
		}
		if (prefabJson["BoxCollider3DComponent"].contains("IsTrigger"))
		{
			buffer.isTrigger = prefabJson["BoxCollider3DComponent"]["IsTrigger"].get<bool>();
		}

		AddPrefabComponent(prefab, buffer);
	}

	if (prefabJson.contains("BarComponent"))
	{
		AddPrefabComponent(prefab, BarComponent());
	}

	if (prefabJson.contains("TextComponent"))
	{
		TextComponent bufferText;
		std::string text = prefabJson["TextComponent"]["Text"].get<std::string>();
		bufferText.text = std::wstring(text.begin(), text.end());

		if (prefabJson["TextComponent"].contains("FontName"))
		{
			std::string bufferStr = prefabJson["TextComponent"]["FontName"].get<std::string>();
			bufferText.fontName = std::wstring(bufferStr.begin(), bufferStr.end());
		}
		if (prefabJson["TextComponent"].contains("FontSize"))
		{
			bufferText.fontSize = prefabJson["TextComponent"]["FontSize"].get<float>();
		}
		if (prefabJson["TextComponent"].contains("Color"))
		{
			bufferText.color = DirectX::XMFLOAT4(
				prefabJson["TextComponent"]["Color"]["r"].get<float>(),
				prefabJson["TextComponent"]["Color"]["g"].get<float>(),
				prefabJson["TextComponent"]["Color"]["b"].get<float>(),
				prefabJson["TextComponent"]["Color"]["a"].get<float>()
			);
		}
		if (prefabJson["TextComponent"].contains("HorizontalAlignment"))
		{
			auto alignStr = prefabJson["TextComponent"]["HorizontalAlignment"].get<std::string>();

			if (alignStr == "Left") bufferText.horizontalAlignment = Alignment::Left;
			else if (alignStr == "Right") bufferText.horizontalAlignment = Alignment::Right;
			else if (alignStr == "Center") bufferText.horizontalAlignment = Alignment::Center;
		}
		if (prefabJson["TextComponent"].contains("VerticalAlignment"))
		{
			auto alignStr = prefabJson["TextComponent"]["VerticalAlignment"].get<std::string>();
			if (alignStr == "Left") bufferText.verticalAlignment = Alignment::Left;
			else if (alignStr == "Right") bufferText.verticalAlignment = Alignment::Right;
			else if (alignStr == "Center") bufferText.verticalAlignment = Alignment::Center;
		}
		if (prefabJson["TextComponent"].contains("OffsetX"))
		{
			bufferText.offsetX = prefabJson["TextComponent"]["OffsetX"].get<float>();
		}
		if (prefabJson["TextComponent"].contains("OffsetY"))
		{
			bufferText.offsetY = prefabJson["TextComponent"]["OffsetY"].get<float>();
		}
		if (prefabJson["TextComponent"].contains("FontWeight"))
		{
			auto weightStr = prefabJson["TextComponent"]["FontWeight"].get<std::string>();
			if (weightStr == "Normal") bufferText.fontWeight = FontWeight::Normal;
			else if (weightStr == "Bold") bufferText.fontWeight = FontWeight::Bold;
			else if (weightStr == "Light") bufferText.fontWeight = FontWeight::Light;
		}
		if (prefabJson["TextComponent"].contains("FontStyle"))
		{
			auto styleStr = prefabJson["TextComponent"]["FontStyle"].get<std::string>();
			if (styleStr == "Normal") bufferText.fontStyle = FontStyle::Normal;
			else if (styleStr == "Italic") bufferText.fontStyle = FontStyle::Italic;
			else if (styleStr == "Oblique") bufferText.fontStyle = FontStyle::Oblique;
		}
		if (prefabJson["TextComponent"].contains("FontStretch"))
		{
			auto stretchStr = prefabJson["TextComponent"]["FontStretch"].get<std::string>();
			if (stretchStr == "Normal") bufferText.fontStretch = FontStretch::Normal;
			else if (stretchStr == "Condensed") bufferText.fontStretch = FontStretch::Condensed;
			else if (stretchStr == "Expanded") bufferText.fontStretch = FontStretch::Expanded;
		}

		AddPrefabComponent(prefab, bufferText);
	}

	//----------------Events------------------//
	if (prefabJson.contains("ResolutionChange"))
	{
		AddPrefabComponent(prefab, ResolutionChange(
			prefabJson["ResolutionChange"]["Width"].get<int>(),
			prefabJson["ResolutionChange"]["Height"].get<int>()
		));
	}

	if (prefabJson.contains("LoadState"))
	{
		AddPrefabComponent(prefab, LoadState(
			GenerateStringID(prefabJson["LoadState"].get<std::string>().c_str())
		));
	}

	if (prefabJson.contains("OpenSettings"))
	{
		AddPrefabComponent(prefab, OpenSettings());
	}

	if (prefabJson.contains("QuitGame"))
	{
		AddPrefabComponent(prefab, QuitGame());
	}

	if (prefabJson.contains("ExitToMainMenu"))
	{
		AddPrefabComponent(prefab, ExitToMainMenu());
	}
	//----------------------------------------//

	//-----------------Tags-------------------//
	if (prefabJson.contains("PlayerTag"))
	{
		AddPrefabComponent(prefab, PlayerTag());
	}

	if (prefabJson.contains("EnemyTag"))
	{
		AddPrefabComponent(prefab, EnemyTag());
	}
	//----------------------------------------//

	if (modelStatic)
	{
		AddPrefabComponent(prefab, StaticModelTag());
	}

	prefabs[prefabName] = prefab;
}

void PrefabManager::ApplyPrefab(Registry& registry, const PrefabName& prefabName, Entity e)
{
	const Prefab& prefab = GetPrefab(prefabName);


	for (const auto& comp : prefab.components)
	{
		comp.apply(registry, e);
	}
}

const Prefab& PrefabManager::GetPrefab(const PrefabName& prefabName) const
{
	if (prefabs.find(prefabName) == prefabs.end())
	{
		throw std::runtime_error("Prefab not found: " + prefabName);
	}
	return prefabs.at(prefabName);
}

PrefabManager::PrefabManager(EngineContext& engineContext) : engineContext(engineContext) {}