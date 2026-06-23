module;
export module Scripts.PlayerMove;

import Engine;
import BasicScript;
import Keyboard;

export namespace Umi
{
    class PlayerMove : public BasicScript
    {
    public:
        void Start() override 
        {

        }

        void Update() override
        {
            Transform& transform = GetComponent<Transform>();

            if (Keyboard::IsKeyDown(KK_W))
            {
                transform.pos.z += 0.1f;
            }
            if (Keyboard::IsKeyDown(KK_S))
            {
                transform.pos.z -= 0.1f;
            }
            if (Keyboard::IsKeyDown(KK_A))
            {
                transform.pos.x -= 0.1f;
            }
            if (Keyboard::IsKeyDown(KK_D))
            {
                transform.pos.x += 0.1f;
            }
            if (Keyboard::IsKeyDown(KK_Q))
            {
                transform.rot.z -= 0.1f;
            }
            if (Keyboard::IsKeyDown(KK_E))
            {
                transform.rot.z += 0.1f;
            }
        }
    };
}
