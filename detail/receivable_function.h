//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <thread>
#include <future>
#include <type_traits>
#include <utility>

#include "message_queue.h"


namespace sam
{
	namespace detail
	{

		template <typename Callable_t, typename ...Arguments_t>
		void receivable_function_template(std::promise<void> &create_message_queue_promise, Callable_t callable, Arguments_t ...arguments);

		template <typename Callable_t, typename ...Arguments_t>
		void (*make_receivable_function())(std::promise<void> &, typename std::decay<Callable_t>::type, typename std::decay<Arguments_t>::type...);
	}
}


namespace sam
{
	namespace detail
	{

		template <typename Callable_t, typename ...Arguments_t>
		void receivable_function_template(std::promise<void> &create_message_queue_promise, Callable_t callable, Arguments_t ...arguments)
		{
			message_queue_guard message_queue_guard(std::this_thread::get_id());
			create_message_queue_promise.set_value();

			callable(std::move(arguments)...);
		}


		template <typename Callable_t, typename ...Arguments_t>
		void (*make_receivable_function())(std::promise<void> &, typename std::decay<Callable_t>::type, typename std::decay<Arguments_t>::type...)
		{
			return receivable_function_template<typename std::decay<Callable_t>::type, typename std::decay<Arguments_t>::type...>;
		}

	}
}
