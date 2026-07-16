module;
export module Scripts.EnemyScript;

import Engine;

export namespace Umi
{
    class EnemyScript : public BasicScript
    {
    public:

		int hp = 5;

        void Start() override
        {
           
        }

        void Update() override
        {
			if (hp <= 0)
			{
                DestroyEntity();
			}
        }

        void GiveDamage(int damage)
        {
            hp -= damage;
        }
    };
}
