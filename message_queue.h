#pragma once

#include "thread_safe_queue.h"
#include "message.h"


namespace sam
{

	namespace details
	{

		void create_message_queue_for_thread(std::thread::id id);
		void remove_message_queue_for_thread(std::thread::id id);

		queue<message> &message_queue_for_thread(std::thread::id id);

	}

}

