#pragma once

#include <thread>
#include <unordered_map>
#include <utility>
#include <memory>

#include "thread_safe_queue.h"
#include "message.h"
#include "handler.h"
#include "signature.h"
#include "message_queue.h"
#include "stuff.h"


namespace sam
{

	//
	// Declarations

	template <typename ...Arguments_t>
	std::function<void (Arguments_t...)> make_receivable(std::function<void (Arguments_t...)> std_function);

	template <typename ...Arguments_t>
	std::function<void (Arguments_t...)> make_receivable(void (*function_pointer)(Arguments_t...));

	template <typename ...Arguments_t>
	void send(std::thread &thread, Arguments_t &&...arguments);

	template <typename ...Callables_t>
	void receive(Callables_t ...callables);


	namespace details
	{

		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...);

		int register_handler(std::unordered_map<signature_t, std::shared_ptr<super_handler>> &handlers, std::shared_ptr<super_handler> handler);

		template <typename ...Callables_t>
		std::unordered_map<signature_t, std::shared_ptr<super_handler>> register_handlers(Callables_t ...callables);

		ctlcode_t default_control_code_handler(ctlcode_t control_code);
		ctlcode_t dispatch_message(const std::unordered_map<signature_t, std::shared_ptr<super_handler>> &handlers, std::shared_ptr<message> message_ptr);

	}


	//
	// Definitions

	template <typename ...Arguments_t>
	std::function<void (Arguments_t...)> make_receivable(std::function<void (Arguments_t...)> std_function)
	{
		auto receivable_function = [std_function](Arguments_t &&...arguments)
		{
			details::create_message_queue_for_thread(std::this_thread::get_id());

			std_function(std::forward<Arguments_t>(arguments)...);

			details::remove_message_queue_for_thread(std::this_thread::get_id());
		};

		return receivable_function;
	}


	template <typename ...Arguments_t>
	std::function<void (Arguments_t...)> make_receivable(void (*function_pointer)(Arguments_t...))
	{
		return make_receivable<Arguments_t...>(std::function<void (Arguments_t...)>(function_pointer));
	}


	template <typename ...Arguments_t>
	void send(std::thread &thread, Arguments_t &&...arguments)
	{
		std::shared_ptr<details::message> message_ptr(details::new_shared_message(std::forward<Arguments_t>(arguments)...));
		details::push_message_for_thread(thread.get_id(), message_ptr);
	}


	template <typename ...Callables_t>
	void receive(Callables_t ...callables)
	{
		auto handlers = details::register_handlers(callables...);
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


	namespace details
	{

		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...)
		{
		}


		template <typename ...Callables_t>
		std::unordered_map<signature_t, std::shared_ptr<super_handler>> register_handlers(Callables_t ...callables)
		{
			std::unordered_map<signature_t, std::shared_ptr<super_handler>> handlers;
			unpack(register_handler(handlers, new_shared_handler(callables))...);

			signature_t ctlcode_handler_signature = new_signature<ctlcode_t>();
			if (handlers.find(ctlcode_handler_signature) == handlers.end())
			{
				register_handler(handlers, new_shared_handler(default_control_code_handler));
			}

			return handlers;
		}

	}

}

