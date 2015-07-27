//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <thread>
#include <unordered_map>
#include <utility>
#include <memory>
#include <type_traits>
#include <future>
#include <chrono>

#include "thread_safe_queue.h"
#include "message.h"
#include "handler.h"
#include "signature.h"
#include "message_queue.h"


namespace sam
{

	//
	// Public
	//


	//
	// Declarations

	template <typename Function_t, typename ...Arguments_t>
	std::thread receivable_thread(Function_t &&function, Arguments_t &&...arguments);

	template <typename ...Functions_t>
	void receive(Functions_t &&...functions);

	template <typename Rep, typename Period, typename ...Functions_t>
	void receive_for(const std::chrono::duration<Rep, Period> &timeout_duration, Functions_t &&...functions);


	class mailbox
	{
	public:
		mailbox(const std::thread &thread);

		template <typename ...Arguments_t>
		void send(Arguments_t &&...arguments);

	private:
		details::msgqueue_t &_message_queue;
	};



	//
	// Private
	//


	//
	// Declarations

	namespace details
	{

		typedef std::unordered_map<signature_t, std::shared_ptr<handler>> handlers_t;


		template <typename Function_t, typename ...Arguments_t>
		void receivable_function_template(std::promise<void> &create_message_queue_promise, Function_t function, Arguments_t ...arguments);

		template <typename Function_t, typename ...Arguments_t>
		void (*make_receivable_function())(std::promise<void> &, typename std::decay<Function_t>::type, typename std::decay<Arguments_t>::type...);

		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...);

		int register_handler(handlers_t &handlers, std::shared_ptr<handler> handler);

		template <typename ...Functions_t>
		handlers_t register_handlers(Functions_t &&...functions);

		template <typename ...Functions_t>
		handlers_t register_handlers_with_timeout(Functions_t &&...functions);

		ctlcode_t default_control_code_handler(ctlcode_t control_code);
		ctlcode_t default_timeout_handler(timeout);
		ctlcode_t dispatch_message(const handlers_t &handlers, std::shared_ptr<message> message_ptr);

	}



	//
	// Public
	//


	//
	// Definitions

	template <typename Function_t, typename ...Arguments_t>
	std::thread receivable_thread(Function_t &&function, Arguments_t &&...arguments)
	{
		auto receivable_function = details::make_receivable_function<Function_t, Arguments_t...>();
		std::promise<void> create_message_queue_promise;
		std::future<void> message_queue_created{create_message_queue_promise.get_future()};
		std::thread thread{receivable_function, std::ref(create_message_queue_promise), std::forward<Function_t>(function), std::forward<Arguments_t>(arguments)...};
		message_queue_created.get();

		return thread;
	}


	template <typename ...Functions_t>
	void receive(Functions_t &&...functions)
	{
		details::handlers_t handlers{details::register_handlers(std::forward<Functions_t>(functions)...)};
		details::msgqueue_t &message_queue{details::message_queue_for_thread(std::this_thread::get_id())};
		for (;;)
		{
			std::shared_ptr<details::message> message_ptr{message_queue.wait_and_pop()};
			ctlcode_t control_code{details::dispatch_message(handlers, message_ptr)};
			if (control_code == STOP)
			{
				break;
			}
		}
	}


	template <typename Rep, typename Period, typename ...Functions_t>
	void receive_for(const std::chrono::duration<Rep, Period> &timeout_duration, Functions_t &&...functions)
	{
		details::handlers_t handlers{details::register_handlers_with_timeout(std::forward<Functions_t>(functions)...)};
		details::msgqueue_t &message_queue{details::message_queue_for_thread(std::this_thread::get_id())};
		for (;;)
		{
			std::shared_ptr<details::message> message_ptr{message_queue.wait_for_and_pop(timeout_duration)};
			if (message_ptr == nullptr)
			{
				message_ptr = details::make_shared_message(timeout{});
			}

			ctlcode_t control_code{details::dispatch_message(handlers, message_ptr)};
			if (control_code == STOP)
			{
				break;
			}
		}
	}


	template <typename ...Arguments_t>
	void mailbox::send(Arguments_t &&...arguments)
	{
		std::shared_ptr<details::message> message_ptr{details::make_shared_message(std::forward<Arguments_t>(arguments)...)};
		_message_queue.push(message_ptr);
	}



	//
	// Private
	//


	//
	// Definitions

	namespace details
	{

		template <typename Function_t, typename ...Arguments_t>
		void receivable_function_template(std::promise<void> &create_message_queue_promise, Function_t function, Arguments_t ...arguments)
		{
			details::message_queue_guard message_queue_guard{std::this_thread::get_id()};
			create_message_queue_promise.set_value();

			function(std::move(arguments)...);
		}


		template <typename Function_t, typename ...Arguments_t>
		void (*make_receivable_function())(std::promise<void> &, typename std::decay<Function_t>::type, typename std::decay<Arguments_t>::type...)
		{
			return details::receivable_function_template<typename std::decay<Function_t>::type, typename std::decay<Arguments_t>::type...>;
		}


		template <typename ...Whatever_t>
		void unpack(Whatever_t &&...)
		{
		}


		template <typename ...Functions_t>
		handlers_t register_handlers(Functions_t &&...functions)
		{
			handlers_t handlers;
			unpack(register_handler(handlers, make_shared_handler(std::forward<Functions_t>(functions)))...);

			signature_t ctlcode_handler_signature{make_signature<ctlcode_t>()};
			if (handlers.find(ctlcode_handler_signature) == handlers.end())
			{
				register_handler(handlers, make_shared_handler(default_control_code_handler));
			}

			return handlers;
		}


		template <typename ...Functions_t>
		handlers_t register_handlers_with_timeout(Functions_t &&...functions)
		{
			handlers_t handlers{register_handlers(std::forward<Functions_t>(functions)...)};

			signature_t timeout_handler_signature{make_signature<timeout>()};
			if (handlers.find(timeout_handler_signature) == handlers.end())
			{
				register_handler(handlers, make_shared_handler(default_timeout_handler));
			}

			return handlers;
		}

	}

}

