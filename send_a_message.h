#pragma once

#include <typeindex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <memory>
#include <cassert>

#include "queue.h"
#include "message.h"
#include "handler.h"


namespace sam
{

	template <typename ...Arguments_t>
	void send(std::thread::id id, Arguments_t &&...arguments);

	template <typename ...Callables_t>
	void receive(Callables_t ...callables);



	namespace details
	{

		std::shared_ptr<queue<message>> &message_queue_for_thread(std::thread::id id);

		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...);

		int insert_handler(std::unordered_map<std::type_index, std::shared_ptr<super_handler>> &handlers, std::shared_ptr<super_handler> handler);

		template <typename ...Callables_t>
		std::unordered_map<std::type_index, std::shared_ptr<super_handler>> register_handlers(Callables_t ...callables);

	}



	template <typename ...Arguments_t>
	void send(std::thread::id id, Arguments_t &&...arguments)
	{
		std::shared_ptr<details::message> message_ptr(details::new_shared_message(std::forward<Arguments_t>(arguments)...));
		auto message_queue = details::message_queue_for_thread(id);
		message_queue->push(message_ptr);
	}


	template <typename ...Callables_t>
	void receive(Callables_t ...callables)
	{
		auto handlers = details::register_handlers(callables...);

		auto message_queue = details::message_queue_for_thread(std::this_thread::get_id());
		for (;;)
		{
			std::shared_ptr<details::message> message = message_queue->wait_and_pop();

			auto iterator = handlers.find(message->signature());
			assert(iterator != handlers.end());

			std::shared_ptr<details::super_handler> handler = iterator->second;
			hretval_t retval = handler->do_call(message->data());
			if (retval == STOP)
			{
				break;
			}
		}
	}



	namespace details
	{

		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...)
		{
		}


		template <typename ...Callables_t>
		std::unordered_map<std::type_index, std::shared_ptr<super_handler>> register_handlers(Callables_t ...callables)
		{
			std::unordered_map<std::type_index, std::shared_ptr<super_handler>> handlers;
			unpack(insert_handler(handlers, new_shared_handler(callables))...);

			return handlers;
		}

	}

}

