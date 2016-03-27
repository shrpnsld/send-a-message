#pragma once

#include <thread>
#include <future>
#include <utility>
#include <functional>

#include "detail/receivable_function.h"


namespace sam
{

	template <typename Callable_t, typename ...Arguments_t>
	std::thread receivable_thread(Callable_t &&callable, Arguments_t &&...arguments);

}


namespace sam
{

	template <typename Callable_t, typename ...Arguments_t>
	std::thread receivable_thread(Callable_t &&callable, Arguments_t &&...arguments)
	{
		auto receivable_function {detail::make_receivable_function<Callable_t, Arguments_t...>()};
		std::promise<void> create_message_queue_promise;
		std::future<void> message_queue_created {create_message_queue_promise.get_future()};
		std::thread thread {receivable_function, std::ref(create_message_queue_promise), std::forward<Callable_t>(callable), std::forward<Arguments_t>(arguments)...};
		message_queue_created.get();

		return thread;
	}

}

