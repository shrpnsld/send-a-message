#include <map>
#include <thread>
#include <mutex>
#include <tuple>
#include <cassert>

#include "message_queue.hpp"


namespace sam { namespace detail
{

	using msgqueuelst_t = std::map<std::thread::id, msgqueue_t>;

	std::mutex _mutex;
	msgqueuelst_t _message_queues;

	msgqueuelst_t::iterator find_message_queue(std::thread::id id);

}
}


namespace sam { namespace detail
{

	message_queue_guard::message_queue_guard(std::thread::id id)
		:
		_id {id}
	{
		std::lock_guard<std::mutex> lock_guard {_mutex};

		_message_queues.emplace(std::piecewise_construct, std::forward_as_tuple(_id), std::forward_as_tuple());
	}


	message_queue_guard::~message_queue_guard()
	try
	{
		std::lock_guard<std::mutex> lock_guard {_mutex};

		_message_queues.erase(_id);
	}
	catch (std::system_error &error)
	{
		assert(false); // : failed to lock mutex
	}


	msgqueue_t &message_queue_for_thread(std::thread::id id)
	{
		auto iterator {find_message_queue(id)};
		assert(iterator != _message_queues.end()); // : no message queue was created for the thread

		return iterator->second;
	}


	msgqueuelst_t::iterator find_message_queue(std::thread::id id)
	{
		std::lock_guard<std::mutex> lock_guard {_mutex};

		return _message_queues.find(id);
	}

}
}

