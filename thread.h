#pragma once

#include <thread>
#include <utility>

#include "message.h"
#include "queue.h"


namespace tfm
{

	class thread
	{
	public:
		thread(std::thread::native_handle_type handle);

		queue<super_message> &message_queue();

	private:
		std::thread::native_handle_type _handle;
		queue<super_message> _message_queue;
	};

}

