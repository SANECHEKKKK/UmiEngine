module;

export module TopBar;

import EngineContext;
import EditorContext;

export namespace Umi
{
    class TopBar
    {
    private:
        EngineContext& engineContext;
        EditorContext& editorContext;
        
    public:
        void Draw();
        
        TopBar(EngineContext& engineContext, EditorContext& editorContext) : engineContext(engineContext), editorContext(editorContext) {};
    };
}