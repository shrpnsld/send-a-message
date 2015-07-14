//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include "thread_safe_queue.h"
#include "message.h"


namespace sam
{

	//
	// Private
	//


	//
	// Declarations

	namespace details
	{

		typedef queue<message> msgqueue_t;

		void create_message_queue_for_thread(std::thread::id id);
		void remove_message_queue_for_thread(std::thread::id id);

		msgqueue_t &message_queue_for_thread(std::thread::id id);

	}

}

