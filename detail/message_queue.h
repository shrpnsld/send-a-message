//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <thread>

#include "thread_safe_queue.h"
#include "message.h"


namespace sam { namespace detail
{

	typedef queue<message> msgqueue_t;

	msgqueue_t &message_queue_for_thread(std::thread::id id);


	class message_queue_guard
	{
	public:
		message_queue_guard(std::thread::id id);
		~message_queue_guard();

	private:
		std::thread::id _id;
	};

}
}

