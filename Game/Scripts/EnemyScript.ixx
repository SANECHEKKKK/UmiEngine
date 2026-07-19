module;
#include <string>
export module Scripts.EnemyScript;

import Engine;
import Text;

export namespace Umi
{
	class EnemyScript : public BasicScript
	{
	public:

		int hp = 5;

		Entity hpText;

		void Start() override
		{
		}

		void Update() override
		{
			if (hp <= 0)
			{
				DestroyEntity();
			}

			hpText = FindEntityWithTag("IMYA");
			if (hpText != INVALID_ENTITY)
				GetComponent<Text>(hpText).text = std::to_string(hp);
		}

		void GiveDamage(int damage)
		{
			hp -= damage;
		}
	};
}
