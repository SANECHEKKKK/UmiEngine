module;
#include <cstdint>
#include <limits>
export module Material;

import Math;

export namespace Umi
{
    struct Material {
        //-----------Texture nums in the buffer-----------
        uint32_t albedoIndex = UINT_MAX;
        uint32_t normalIndex = UINT_MAX;
        uint32_t roughnessIndex = UINT_MAX;
        uint32_t metallicIndex = UINT_MAX;
        //------------------------------------------------

        //---------Properties for custom materials--------
        float4 baseColor = { 1, 1, 1, 1 };
        float roughness = 0.5f;  // overridden by roughness map if present
        float metallic = 0.0f;  // overridden by metallic map if present
        float emissive = 0.0f;
        float opacity = 1.0f;
        //------------------------------------------------
    };
}