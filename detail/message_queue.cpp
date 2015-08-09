//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#include <map>
#include <thread>
#include <mutex>
#include <cassert>

#include "message_queue.h"


namespace sam { namespace detail
{

	typedef std::map<std::thread::id, queue<message>> msgqueuelst_t;

	std::mutex _mutex;
	msgqueuelst_t _message_queues;

	msgqueuelst_t::iterator find_message_queue(std::thread::id id);

}
}


namespace sam { namespace detail
{

	message_queue_guard::message_queue_guard(std::thread::id id) :
		_id(id)
	{
		std::lock_guard<std::mutex> lock_guard(_mutex);

		_message_queues.emplace(_id, queue<message>());
	}


	message_queue_guard::~message_queue_guard()
	{
		std::lock_guard<std::mutex> lock_guard(_mutex);

		_message_queues.erase(_id);
	}


	msgqueue_t &message_queue_for_thread(std::thread::id id)
	{
		auto iterator = find_message_queue(id);
		assert(iterator != _message_queues.end()); // if fails - no message queue was created for the thread

		return iterator->second;
	}


	msgqueuelst_t::iterator find_message_queue(std::thread::id id)
	{
		std::lock_guard<std::mutex> lock_guard(_mutex);

		return _message_queues.find(id);
	}

}
}

