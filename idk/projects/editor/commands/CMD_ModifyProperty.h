#include <editor/commands/ICommand.h>
#include <core/GameObject.h>
#include <reflect/reflect.h>

namespace idk {
	class CMD_ModifyProperty
		: public ICommand
    {
	public:
		CMD_ModifyProperty(GenericHandle component, string_view property_path, reflect::dynamic old_value, reflect::dynamic new_value);
		virtual bool execute() override;
		virtual bool undo() override;

	private:
		GenericHandle component;
        string property_path;
        reflect::dynamic old_value;
        reflect::dynamic new_value;
	};
}
