// AUTO-GENERATED — do not edit.
import ScriptManager;
import Script;
import Scripts.DebugScript;
import Scripts.EnemyScript;
import Scripts.PlayerMove;

void RegisterAllScripts(Umi::ScriptManager& sm)
{
    sm.RegisterScript<Umi::DebugScript>("DebugScript");
    sm.RegisterScript<Umi::EnemyScript>("EnemyScript");
    sm.RegisterScript<Umi::PlayerMove>("PlayerMove");
}

extern "C" __declspec(dllexport) void RegisterScripts(Umi::ScriptManager& sm)
{
    RegisterAllScripts(sm);
}
