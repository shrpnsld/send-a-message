#pragma once

#include <thread>
#include <future>
#include <type_traits>
#include <utility>

#include "message_queue.hpp"


namespace sam { namespace detail
{

	//
	// Module public

	template <typename Callable_t, typename ...Arguments_t>
	using recvblfunc_t = void (*)(std::promise<void> &, typename std::decay<Callable_t>::type, typename std::decay<Arguments_t>::type...);


	template <typename Callable_t, typename ...Arguments_t>
	constexpr recvblfunc_t<Callable_t, Arguments_t...> make_receivable_function() noexcept;


	//
	// Module private

	template <typename Callable_t, typename ...Arguments_t>
	void receivable_function_template(std::promise<void> &create_message_queue_promise, Callable_t callable, Arguments_t ...arguments);

}
}


namespace sam { namespace detail
{

	template <typename Callable_t, typename ...Arguments_t>
	constexpr recvblfunc_t<Callable_t, Arguments_t...> make_receivable_function() noexcept
	{
		return receivable_function_template<typename std::decay<Callable_t>::type, typename std::decay<Arguments_t>::type...>;
	}


	template <typename Callable_t, typename ...Arguments_t>
	void receivable_function_template(std::promise<void> &create_message_queue_promise, Callable_t callable, Arguments_t ...arguments)
	{
		message_queue_guard message_queue_guard {std::this_thread::get_id()};
		create_message_queue_promise.set_value();

		callable(std::move(arguments)...);
	}

}
}

