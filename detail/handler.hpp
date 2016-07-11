#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>

#include "../flags.hpp"
#include "apply.hpp"


namespace sam { namespace detail
{

	//
	// Module public

	using handler_t = std::function<ctlcode_t (void *)>;


	template <typename ...Arguments_t>
	handler_t make_handler(ctlcode_t (*function_pointer)(Arguments_t...)) noexcept;

	template <typename ...Arguments_t>
	handler_t make_handler(void (*function_pointer)(Arguments_t...)) noexcept;

	template <typename Callable_t>
	handler_t make_handler(Callable_t &&callable) noexcept;


	//
	// Module private

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, void (Callable_t::*)(Arguments_t...) const) noexcept;

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, ctlcode_t (Callable_t::*)(Arguments_t...) const) noexcept;

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_ctlcode_callable(Callable_t callable) noexcept;

	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_void_callable(Callable_t callable) noexcept;

}
}


namespace sam { namespace detail
{

	template <typename ...Arguments_t>
	handler_t make_handler(ctlcode_t (*function_pointer)(Arguments_t...)) noexcept
	{
		using ctlcodefunc_t = ctlcode_t (*)(Arguments_t...);

		return make_handler_from_ctlcode_callable<ctlcodefunc_t &, Arguments_t...>(function_pointer);
	}


	template <typename ...Arguments_t>
	handler_t make_handler(void (*function_pointer)(Arguments_t...)) noexcept
	{
		using voidfunc_t = void (*)(Arguments_t...);

		return make_handler_from_void_callable<voidfunc_t &, Arguments_t...>(function_pointer);
	}


	template <typename Callable_t>
	handler_t make_handler(Callable_t &&callable) noexcept
	{
		return make_handler_from_function_object(std::forward<Callable_t>(callable), &Callable_t::operator ());
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, ctlcode_t (Callable_t::*)(Arguments_t...) const) noexcept
	{
		return make_handler_from_ctlcode_callable<Callable_t, Arguments_t...>(std::forward<Callable_t>(callable));
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_function_object(Callable_t &&callable, void (Callable_t::*)(Arguments_t...) const) noexcept
	{
		return make_handler_from_void_callable<Callable_t, Arguments_t...>(std::forward<Callable_t>(callable));
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_ctlcode_callable(Callable_t callable) noexcept
	{
		return {
			[callable](void *context) -> ctlcode_t
			{
				using arguments_t = std::tuple<Arguments_t...>;

				arguments_t &arguments {*reinterpret_cast<arguments_t *>(context)};
				return apply_tuple<ctlcode_t>(callable, arguments);
			}
		};
	}


	template <typename Callable_t, typename ...Arguments_t>
	handler_t make_handler_from_void_callable(Callable_t callable) noexcept
	{
		return {
			[callable](void *context) -> ctlcode_t
			{
				using arguments_t = std::tuple<Arguments_t...>;

				arguments_t &arguments {*reinterpret_cast<arguments_t *>(context)};
				apply_tuple<void>(callable, arguments);
				return CONTINUE;
			}
		};
	}

}
}

