#pragma once

#include <thread>

#include "thread_safe_queue.hpp"
#include "message.hpp"


namespace sam { namespace detail
{

	//
	// Module public

	using msgqueue_t = ts_queue<message>;


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

