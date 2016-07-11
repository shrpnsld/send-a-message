#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "../flags.hpp"
#include "signature.hpp"
#include "handler.hpp"
#include "message.hpp"


namespace sam { namespace detail
{

	//
	// Module public

	using handlers_t = std::unordered_map<signature_t, handler_t>;


	template <typename ...Callables_t>
	handlers_t register_handlers(Callables_t &&...callables);

	template <typename ...Callables_t>
	handlers_t register_handlers_with_timeout(Callables_t &&...callables);


	//
	// Module private

	template <typename ...Whatever_t>
	void unpack(Whatever_t &&...) noexcept;

	template <typename Callable_t>
	int register_handler(handlers_t &handlers, Callable_t &&callable);

	ctlcode_t default_control_code_handler(ctlcode_t control_code) noexcept;
	ctlcode_t default_timeout_handler(timeout_error) noexcept;
	ctlcode_t dispatch_message(const handlers_t &handlers, std::shared_ptr<message> message_ptr);

}
}


namespace sam { namespace detail
{

	template <typename ...Callables_t>
	handlers_t register_handlers(Callables_t &&...callables)
	{
		handlers_t handlers;
		unpack(register_handler(handlers, std::forward<Callables_t>(callables))...);

		signature_t ctlcode_handler_signature {make_signature<ctlcode_t>()};
		if (handlers.find(ctlcode_handler_signature) == handlers.end())
		{
			register_handler(handlers, default_control_code_handler);
		}

		return handlers;
	}


	template <typename ...Callables_t>
	handlers_t register_handlers_with_timeout(Callables_t &&...callables)
	{
		handlers_t handlers {register_handlers(std::forward<Callables_t>(callables)...)};

		signature_t timeout_handler_signature {make_signature<timeout_error>()};
		if (handlers.find(timeout_handler_signature) == handlers.end())
		{
			register_handler(handlers, default_timeout_handler);
		}

		return handlers;
	}


	template <typename ...Whatever_t>
	void unpack(Whatever_t &&...) noexcept
	{
	}


	template <typename Callable_t>
	int register_handler(handlers_t &handlers, Callable_t &&callable)
	{
		auto signature {make_signature(callable)};
		auto handler {make_handler(std::forward<Callable_t>(callable))};
		handlers.insert(std::make_pair(signature, handler));

		return 0;
	}

}
}

