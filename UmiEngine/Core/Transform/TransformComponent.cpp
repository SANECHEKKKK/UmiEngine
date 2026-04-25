#include "TransformComponent.h"

using namespace Umi;

float TransformUiComponent::getLeft() const noexcept
{
    return (pos.x - (size.x * scale.x) / 2.0f);
}

float TransformUiComponent::getRight() const noexcept
{
    return (pos.x + (size.x * scale.x) / 2.0f);
}

float TransformUiComponent::getTop() const noexcept
{
    return (pos.y - (size.y * scale.y) / 2.0f);
}

float TransformUiComponent::getBottom() const noexcept
{
    return (pos.y + (size.y * scale.y) / 2.0f);
}
