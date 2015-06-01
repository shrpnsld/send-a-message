#include "thread.h"


namespace tfm
{

	thread::thread(std::thread::native_handle_type handle)
		: _handle(handle)
	{
	}


	queue<super_message> &thread::message_queue()
	{
		return _message_queue;
	}

}

