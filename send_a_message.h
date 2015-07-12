#pragma once

#include <thread>
#include <unordered_map>
#include <utility>
#include <memory>
#include <type_traits>

#include "thread_safe_queue.h"
#include "message.h"
#include "handler.h"
#include "signature.h"
#include "message_queue.h"


namespace sam
{

	//
	// Declarations

	template <typename Function_t, typename ...Arguments_t>
	std::thread receivable_thread(Function_t function, Arguments_t &&...arguments);

	template <typename ...Functions_t>
	void receive(Functions_t ...functions);


	class mailbox
	{
	public:
		mailbox(const std::thread &thread);

		template <typename ...Arguments_t>
		void send(Arguments_t &&...arguments);

	private:
		details::queue<details::message> &_message_queue;
	};


	namespace details
	{

		template <typename Function_t, typename ...Arguments_t>
		void receivable_thread_function(Function_t function, Arguments_t ...arguments);

		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...);

		int register_handler(std::unordered_map<signature_t, std::shared_ptr<handler>> &handlers, std::shared_ptr<handler> handler);

		template <typename ...Functions_t>
		std::unordered_map<signature_t, std::shared_ptr<handler>> register_handlers(Functions_t ...functions);

		ctlcode_t default_control_code_handler(ctlcode_t control_code);
		ctlcode_t dispatch_message(const std::unordered_map<signature_t, std::shared_ptr<handler>> &handlers, std::shared_ptr<message> message_ptr);

	}


	//
	// Definitions

	template <typename Function_t, typename ...Arguments_t>
	std::thread receivable_thread(Function_t function, Arguments_t &&...arguments)
	{
		auto receivable_thread_function_instance = details::receivable_thread_function<Function_t, typename std::decay<Arguments_t>::type...>;
		std::thread thread(receivable_thread_function_instance, function, std::forward<Arguments_t>(arguments)...);
		details::create_message_queue_for_thread(thread.get_id());

		return thread;
	}


	template <typename ...Functions_t>
	void receive(Functions_t ...functions)
	{
		auto handlers = details::register_handlers(functions...);
		for (;;)
		{
			std::shared_ptr<details::message> message_ptr = details::pop_message_for_this_thread();
			ctlcode_t control_code = details::dispatch_message(handlers, message_ptr);
			if (control_code == STOP)
			{
				break;
			}
		}
	}


	template <typename ...Arguments_t>
	void mailbox::send(Arguments_t &&...arguments)
	{
		std::shared_ptr<details::message> message_ptr(details::new_shared_message(std::forward<typename std::decay<Arguments_t>::type>(arguments)...));
		_message_queue.push(message_ptr);
	}


	namespace details
	{

		template <typename Function_t, typename ...Arguments_t>
		void receivable_thread_function(Function_t function, Arguments_t ...arguments)
		{
			function(std::move(arguments)...);
			details::remove_message_queue_for_thread(std::this_thread::get_id());
		}


		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...)
		{
		}


		template <typename ...Functions_t>
		std::unordered_map<signature_t, std::shared_ptr<handler>> register_handlers(Functions_t ...functions)
		{
			std::unordered_map<signature_t, std::shared_ptr<handler>> handlers;
			unpack(register_handler(handlers, new_shared_handler(functions))...);

			signature_t ctlcode_handler_signature = new_signature<ctlcode_t>();
			if (handlers.find(ctlcode_handler_signature) == handlers.end())
			{
				register_handler(handlers, new_shared_handler(default_control_code_handler));
			}

			return handlers;
		}

	}

}

