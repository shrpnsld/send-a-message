#pragma once

#include "thread_safe_queue.h"
#include "message.h"


namespace sam
{

	namespace details
	{

		typedef queue<message> msgqueue_t;

		void create_message_queue_for_thread(std::thread::id id);
		void remove_message_queue_for_thread(std::thread::id id);

		msgqueue_t &message_queue_for_thread(std::thread::id id);

	}

}

