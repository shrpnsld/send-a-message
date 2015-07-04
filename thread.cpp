#include "thread.h"


namespace sam
{

	namespace details
	{

		thread::thread()
			: _message_queue()
		{
		}


		thread::thread(thread &&another)
			: _message_queue(std::move(another._message_queue))
		{
		}


		queue<message> &thread::message_queue()
		{
			return _message_queue;
		}

	}

}

