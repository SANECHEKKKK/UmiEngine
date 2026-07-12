// AUTO-GENERATED — do not edit.
import ScriptManager;
import BasicScript;
import Scripts.PlayerMove;

void RegisterAllScripts(Umi::ScriptManager& sm)
{
    sm.RegisterScript<Umi::PlayerMove>("PlayerMove");
}

extern "C" __declspec(dllexport) void RegisterScripts(Umi::ScriptManager& sm)
{
    RegisterAllScripts(sm);
}
