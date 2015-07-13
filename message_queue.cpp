#include <map>
#include <thread>
#include <mutex>

#include "message_queue.h"


namespace sam
{

	namespace details
	{

		std::mutex _mutex;
		std::map<std::thread::id, queue<message>> _message_queues;


		void create_message_queue_for_thread(std::thread::id id)
		{
			std::lock_guard<std::mutex> lock_guard{_mutex};

			_message_queues.emplace(id, queue<message>{});
		}


		void remove_message_queue_for_thread(std::thread::id id)
		{
			std::lock_guard<std::mutex> lock_guard{_mutex};

			_message_queues.erase(id);
		}


		msgqueue_t &message_queue_for_thread(std::thread::id id)
		{
			std::lock_guard<std::mutex> lock_guard{_mutex};

			auto iterator = _message_queues.find(id);
			assert(iterator != _message_queues.end()); // if fails - no message queue was created for the thread

			return iterator->second;
		}

	}

}

