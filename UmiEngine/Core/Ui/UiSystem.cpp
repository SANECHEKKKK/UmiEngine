#include <Ui/UiSystem.h>
#include <Registry/Registry.h>
#include <Ui/UiComponent.h>
#include <Transform/TransformComponent.h>
#include <Resolution/Resolution.h>

using namespace Umi;

Vector2 GetAnchorPosition(Anchor anchor)
{
	switch (anchor)
	{
	case Anchor::TopLeft:		return Vector2(0, 0);
	case Anchor::TopCenter:		return Vector2(0.5f, 0);
	case Anchor::TopRight:		return Vector2(1, 0);

	case Anchor::MiddleLeft:	return Vector2(0, 0.5f);
	case Anchor::MiddleCenter:	return Vector2(0.5f, 0.5f);
	case Anchor::MiddleRight:	return Vector2(1, 0.5f);

	case Anchor::BottomLeft:	return Vector2(0, 1);
	case Anchor::BottomCenter:	return Vector2(0.5f, 1);
	case Anchor::BottomRight:	return Vector2(1, 1);
	default:
		return Vector2(0, 0); // Default case, should not happen
		break;
	}
	return Vector2();
}

void UiSystem::RecalculateAll(const Resolution& resolution) noexcept
{
	for (auto e : registry.View<UiComponent, TransformUiComponent>())
	{
		auto& uiComponent = registry.GetComponent<UiComponent>(e);
		auto& transform = registry.GetComponent<TransformUiComponent>(e);

		CalculatePosition(uiComponent, transform, resolution);
	}
}

void UiSystem::CalculatePosition(UiComponent& uiComponent, TransformUiComponent& transform,const Resolution& resolution)
{
	Vector2 anchorPos = GetAnchorPosition(uiComponent.anchor);
	Vector2 size = { resolution.width * uiComponent.width, resolution.height * uiComponent.height };

	Vector2 position = Vector2(
		anchorPos.x * resolution.width + resolution.width * uiComponent.offsetX,
		anchorPos.y * resolution.height + resolution.height * uiComponent.offsetY
	);

	if (uiComponent.anchor == Anchor::TopLeft || uiComponent.anchor == Anchor::MiddleLeft || uiComponent.anchor == Anchor::BottomLeft)
	{
		position.x += size.x / 2.0f; //left anchors
	}
	else if (uiComponent.anchor == Anchor::TopRight || uiComponent.anchor == Anchor::MiddleRight || uiComponent.anchor == Anchor::BottomRight)
	{
		position.x -= size.x / 2.0f; //right anchors
	}
	if (uiComponent.anchor == Anchor::TopLeft || uiComponent.anchor == Anchor::TopCenter || uiComponent.anchor == Anchor::TopRight)
	{
		position.y += size.y / 2.0f; //top anchors
	}
	else if (uiComponent.anchor == Anchor::BottomLeft || uiComponent.anchor == Anchor::BottomCenter || uiComponent.anchor == Anchor::BottomRight)
	{
		position.y -= size.y / 2.0f; //bottom anchors
	}

	transform.pos = position;
	transform.size = size;
}

UiSystem::UiSystem(Registry& registry) : registry(registry) {}