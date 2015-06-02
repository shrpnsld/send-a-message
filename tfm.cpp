#include <map>
#include <cassert>

#include "tfm.h"

#include "pretty_output.h"


namespace tfm
{

	//
	// Definitions

	receive::receive(std::initializer_list<super_handler *> handlers)
	{
		for (auto handler : handlers)
		{
			_handlers.insert(std::make_pair(handler->signature(), std::unique_ptr<super_handler>(handler)));
		}
	}


	void receive::dispatch(std::unique_ptr<super_message> message) const
	{$f
		auto iterator = _handlers.find(message->signature());

		//$w(message->signature().name());
		//for (auto &pair : _handlers)
		//{
		//	$w(pair.second->signature().name());
		//}

		assert(iterator != _handlers.end());

		iterator->second->do_call(message->data());
	}


	std::map<std::thread::native_handle_type, thread> _threads;


	thread &thread_for_handle(std::thread::native_handle_type handle)
	{
		auto iterator = _threads.find(handle);
		assert(iterator != _threads.end());

		return iterator->second;
	}

}

