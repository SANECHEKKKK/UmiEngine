#include <Button/ButtonSystem.h>
#include <Button/ButtonComponent.h>
#include <Texture/TextureComponent.h>
#include <Transform/TransformComponent.h>
#include <Input/InputManager.h>
#include <Layer/LayerComponent.h>
#include <CheckBox/CheckBox.h>
#include <StateRegistry/StateID.h>

#include <Events/ResolutionChange/ResolutionChange.h>
#include <Events/LoadState/LoadState.h>
#include <Events/OpenSettings/OpenSettings.h>
#include <Events/QuitGame/QuitGame.h>
#include <Events/VsyncToggle/VSyncToggle.h>
#include <Events/ExitToMainMenu/ExitToMainMenu.h>

using namespace Umi;

Signal ButtonSystem::HandleButtonClick(Entity e)
{
	if (registry.HasComponent<ResolutionChange>(e))
		CheckCreateButtonEvent<ResolutionChange>(e);

	if (registry.HasComponent<LoadState>(e))
		CheckCreateButtonEvent<LoadState>(e);

	if (registry.HasComponent<OpenSettings>(e))
		CheckCreateButtonEvent<OpenSettings>(e);

	if (registry.HasComponent<QuitGame>(e))
		CheckCreateButtonEvent<QuitGame>(e);

	if (registry.HasComponent<VsyncToggle>(e))
		CheckCreateButtonEvent<VsyncToggle>(e);

	if (registry.HasComponent<ExitToMainMenu>(e))
		CheckCreateButtonEvent<ExitToMainMenu>(e);

	return Signal();
}

Signal ButtonSystem::Update(StringID stateID)
{
	Entity hoveredEntity = _UI32_MAX;
	int highestLayer = INT_MIN;

	for (auto e : registry.View<ButtonComponent, TextureComponent, TransformUiComponent, LayerComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);
		if (stateIDComponent.stateID != stateID)
			continue;

		auto& buttonComponent = registry.GetComponent<ButtonComponent>(e);
		auto& textureComponent = registry.GetComponent<TextureComponent>(e);
		auto& transformUiComponent = registry.GetComponent<TransformUiComponent>(e);
		auto& layerComponent = registry.GetComponent<LayerComponent>(e);

		bool isMouseOver =
			inputManager.mouse.x >= transformUiComponent.getLeft() &&
			inputManager.mouse.x <= transformUiComponent.getRight() &&
			inputManager.mouse.y >= transformUiComponent.getTop() &&
			inputManager.mouse.y <= transformUiComponent.getBottom();

		if (isMouseOver)
		{
			if (layerComponent.layer > highestLayer)
			{
				highestLayer = layerComponent.layer;
				hoveredEntity = e;
			}
		}
		else
		{
			buttonComponent.state = ButtonState::Normal;
			textureComponent.id = buttonComponent.textures[ButtonState::Normal];
		}
	}

	for (auto e : registry.View<CheckBoxComponent, TextureComponent, TransformUiComponent, LayerComponent>())
	{
		auto& checkBoxComponent = registry.GetComponent<CheckBoxComponent>(e);
		auto& textureComponent = registry.GetComponent<TextureComponent>(e);
		auto& transformUiComponent = registry.GetComponent<TransformUiComponent>(e);
		auto& layerComponent = registry.GetComponent<LayerComponent>(e);

		bool isMouseOver =
			inputManager.mouse.x >= transformUiComponent.getLeft() &&
			inputManager.mouse.x <= transformUiComponent.getRight() &&
			inputManager.mouse.y >= transformUiComponent.getTop() &&
			inputManager.mouse.y <= transformUiComponent.getBottom();

		if (isMouseOver)
		{
			if (layerComponent.layer > highestLayer)
			{
				highestLayer = layerComponent.layer;
				hoveredEntity = e;
			}
		}

	}

	if (hoveredEntity != _UI32_MAX)
	{
		auto& textureComponent = registry.GetComponent<TextureComponent>(hoveredEntity);

		//Handle ButtonComponent
		if (registry.HasComponent<ButtonComponent>(hoveredEntity))
		{
			auto& buttonComponent = registry.GetComponent<ButtonComponent>(hoveredEntity);

			if (inputManager.mouse.LeftIsTriggered())
			{
				buttonComponent.state = ButtonState::Pressed;
				textureComponent.id = buttonComponent.textures[ButtonState::Pressed];
				HandleButtonClick(hoveredEntity);
			}
			else if (inputManager.mouse.LeftIsPressed())
			{
				buttonComponent.state = ButtonState::Pressed;
				textureComponent.id = buttonComponent.textures[ButtonState::Pressed];
			}
			else
			{
				buttonComponent.state = ButtonState::Hovered;
				textureComponent.id = buttonComponent.textures[ButtonState::Hovered];
			}
		}
		//Handle CheckBoxComponent
		else if (registry.HasComponent<CheckBoxComponent>(hoveredEntity))
		{
			auto& checkBoxComponent = registry.GetComponent<CheckBoxComponent>(hoveredEntity);

			if (inputManager.mouse.LeftIsTriggered())
			{
				//Toggle checkbox state
				checkBoxComponent.isChecked = !checkBoxComponent.isChecked;
				textureComponent.id = checkBoxComponent.isChecked ?
					checkBoxComponent.textures[CheckBoxState::Checked] : checkBoxComponent.textures[CheckBoxState::Unchecked];
				HandleButtonClick(hoveredEntity);
			}
		}
	}

	return Signal();
}