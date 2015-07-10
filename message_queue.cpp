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
			std::lock_guard<std::mutex> lock_guard(_mutex);

			_message_queues.emplace(id, queue<message>());
		}


		void remove_message_queue_for_thread(std::thread::id id)
		{
			std::lock_guard<std::mutex> lock_guard(_mutex);

			_message_queues.erase(id);
		}


		void push_message_for_thread(std::thread::id id, std::shared_ptr<message> message_ptr)
		{
			auto &message_queue = _message_queues[id];
			message_queue.push(message_ptr);

		}


		std::shared_ptr<message> pop_message_for_this_thread()
		{
			auto &message_queue = _message_queues[std::this_thread::get_id()];
			return message_queue.wait_and_pop();
		}

	}

}

