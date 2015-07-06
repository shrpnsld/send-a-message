#include <map>
#include <mutex>

#include "queue.h"
#include "send_a_message.h"


namespace sam
{

	namespace details
	{

		//
		// Declarations

		std::mutex _mutex;
		std::map<std::thread::id, std::shared_ptr<queue<message>>> _threads;


		//
		// Definitions

		std::shared_ptr<queue<message>> &message_queue_for_thread(std::thread::id id)
		{
			std::lock_guard<std::mutex> lock_guard(_mutex);

			auto iterator = _threads.find(id);
			if (iterator == _threads.end())
			{
				auto pair = _threads.insert(std::make_pair(id, std::shared_ptr<queue<message>>(new queue<message>())));

				return pair.first->second;
			}

			return iterator->second;
		}


		void remove_message_queue_for_thread(std::thread::id id)
		{
			std::lock_guard<std::mutex> lock_guard(_mutex);

			_threads.erase(id);
		}


		int insert_handler(std::unordered_map<std::type_index, std::shared_ptr<super_handler>> &handlers, std::shared_ptr<super_handler> handler)
		{
			handlers.insert(std::make_pair(handler->signature(), handler));
			return 0;
		}

	}

}

