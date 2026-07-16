module;
export module Scripts.DebugScript;

import Engine;

export namespace Umi
{
    class DebugScript : public BasicScript
    {
    public:
        Entity player;

        void Start() override
        {
            player = FindEntitiesWithTag("Player");
        }

        void Update() override
        {
            auto& playerTransform = GetComponent<Transform>(player);

            transform().pos = playerTransform.pos + -playerTransform.Forward() * 1.5f;
        }
    };
}
