//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>

#include "../flags.h"
#include "apply.h"


namespace sam { namespace detail
{

	typedef std::function<ctlcode_t (void *)> handler_t;


	template <typename ...Arguments_t>
	handler_t make_handler(ctlcode_t (*function_pointer)(Arguments_t...));

	template <typename ...Arguments_t>
	handler_t make_handler(void (*function_pointer)(Arguments_t...));

	template <typename Callable_t>
	handler_t make_handler(Callable_t &&callable);


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, void (Callable_t::*)(Arguments_t...) const);

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, ctlcode_t (Callable_t::*)(Arguments_t...) const);

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_ctlcode_callable(Callable_t callable);

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_void_callable(Callable_t callable);

}
}


namespace sam { namespace detail
{

	template <typename ...Arguments_t>
	handler_t make_handler(ctlcode_t (*function_pointer)(Arguments_t...))
	{
		return make_handler_from_ctlcode_callable<ctlcode_t (*&)(Arguments_t...), Arguments_t...>(function_pointer);
	}


	template <typename ...Arguments_t>
	handler_t make_handler(void (*function_pointer)(Arguments_t...))
	{
		return make_handler_from_void_callable<void (*&)(Arguments_t...), Arguments_t...>(function_pointer);
	}


	template <typename Callable_t>
	handler_t make_handler(Callable_t &&callable)
	{
		return make_handler_from_function_object(std::forward<Callable_t>(callable), &Callable_t::operator ());
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, ctlcode_t (Callable_t::*)(Arguments_t...) const)
	{
		return make_handler_from_ctlcode_callable<Callable_t, Arguments_t...>(std::forward<Callable_t>(callable));
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, void (Callable_t::*)(Arguments_t...) const)
	{
		return make_handler_from_void_callable<Callable_t, Arguments_t...>(std::forward<Callable_t>(callable));
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_ctlcode_callable(Callable_t callable)
	{
		handler_t handler = [callable](void *context) -> ctlcode_t
		{
			typedef std::tuple<Arguments_t...> arguments_t;

			arguments_t &arguments = *reinterpret_cast<arguments_t *>(context);
			return apply<ctlcode_t>(callable, arguments);
		};

		return handler;
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_void_callable(Callable_t callable)
	{
		handler_t handler = [callable](void *context) -> ctlcode_t
		{
			typedef std::tuple<Arguments_t...> arguments_t;

			arguments_t &arguments = *reinterpret_cast<arguments_t *>(context);
			apply<void>(callable, arguments);
			return CONTINUE;
		};

		return handler;
	}

}
}

