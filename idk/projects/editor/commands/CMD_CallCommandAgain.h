//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CallCommandAgain.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 NOV 2019
//@brief	Repeats commands again when undo or redo is called.
/*			Place this after calling the command with the number of times it is called.
			EG: 

			for (loop done 5 times) {
				COMMAND();
			}
			CallCommandAgain(5);

			This command directy overrides the CommandController
*/
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_CallCommandAgain : public ICommand { //serialize/deserialize use text.h
	public:
		CMD_CallCommandAgain(int counts); //Create empty on parented

		virtual bool execute() override;

		virtual bool undo() override;

		vector<unique_ptr<ICommand>> repeated_commands{};
	private:
		int num_of_times_to_repeat;
		bool first_call = false;

	};

}