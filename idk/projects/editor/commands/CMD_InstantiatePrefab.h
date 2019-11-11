#include <editor/commands/ICommand.h>
#include <core/GameObject.h>
#include <res/ResourceHandle.h>

namespace idk
{

    class CMD_InstantiatePrefab : public ICommand 
    {
    public:
        CMD_InstantiatePrefab(RscHandle<Prefab> prefab, vec3 pos);

        virtual bool execute() override;
        virtual bool undo() override;

    private:
        Handle<GameObject> _handle;
        RscHandle<Prefab> _prefab;
        vec3 _pos;
    };

}