#pragma once

#include <map>

#include "thread_safe_queue.h"
#include "message.h"


namespace sam
{

	namespace details
	{

		queue<message> &message_queue_for_thread(std::thread::id id);
		void remove_message_queue_for_thread(std::thread::id id);

		void push_message_for_thread(std::thread::id id, std::shared_ptr<message> message_ptr);
		std::shared_ptr<message> pop_message_for_this_thread();

	}

}

