module;
#include <cstdint>
#include <limits>
export module Model;

export namespace Umi
{
    enum class ModelID : uint32_t {};

    // If you still want a sentinel (valid choice in engine code):
    inline constexpr ModelID INVALID_MODELID = static_cast<ModelID>(std::numeric_limits<uint32_t>::max());

    struct Model {
        ModelID id = INVALID_MODELID;
    };
}