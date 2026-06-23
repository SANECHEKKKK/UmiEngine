// AUTO-GENERATED — do not edit.
import ScriptManager;
import BasicScript;
import Scripts.jopes;
import Scripts.PlayerMove;
import Scripts.TEST;
import Scripts.TESTULA;

void RegisterAllScripts(Umi::ScriptManager& sm)
{
    sm.RegisterScript<Umi::jopes>("jopes");
    sm.RegisterScript<Umi::PlayerMove>("PlayerMove");
    sm.RegisterScript<Umi::TEST>("TEST");
    sm.RegisterScript<Umi::TESTULA>("TESTULA");
}

extern "C" __declspec(dllexport) void RegisterScripts(Umi::ScriptManager& sm)
{
    RegisterAllScripts(sm);
}
