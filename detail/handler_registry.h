//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "../flags.h"
#include "signature.h"
#include "handler.h"
#include "message.h"


namespace sam { namespace detail
{

	typedef std::unordered_map<signature_t, handler_t> handlers_t;


	template <typename ...Whatever_t>
	void unpack(Whatever_t &&...);

	template <typename Callable_t>
	int register_handler(handlers_t &handlers, Callable_t &&callable);

	template <typename ...Callables_t>
	handlers_t register_handlers(Callables_t &&...callables);

	template <typename ...Callables_t>
	handlers_t register_handlers_with_timeout(Callables_t &&...callables);

	ctlcode_t default_control_code_handler(ctlcode_t control_code);
	ctlcode_t default_timeout_handler(timeout_error);
	ctlcode_t dispatch_message(const handlers_t &handlers, std::shared_ptr<message> message_ptr);

}
}


namespace sam { namespace detail
{

	template <typename ...Whatever_t>
	void unpack(Whatever_t &&...)
	{
	}


	template <typename Callable_t>
	int register_handler(handlers_t &handlers, Callable_t &&callable)
	{
		signature_t signature = make_signature(std::forward<Callable_t>(callable));
		handler_t handler = make_handler(std::forward<Callable_t>(callable));
		handlers.insert(std::make_pair(signature, handler));
		return 0;
	}


	template <typename ...Callables_t>
	handlers_t register_handlers(Callables_t &&...callables)
	{
		handlers_t handlers;
		unpack(register_handler(handlers, std::forward<Callables_t>(callables))...);

		signature_t ctlcode_handler_signature = make_signature<ctlcode_t>();
		if (handlers.find(ctlcode_handler_signature) == handlers.end())
		{
			register_handler(handlers, default_control_code_handler);
		}

		return handlers;
	}


	template <typename ...Callables_t>
	handlers_t register_handlers_with_timeout(Callables_t &&...callables)
	{
		handlers_t handlers = register_handlers(std::forward<Callables_t>(callables)...);

		signature_t timeout_handler_signature(make_signature<timeout_error>());
		if (handlers.find(timeout_handler_signature) == handlers.end())
		{
			register_handler(handlers, default_timeout_handler);
		}

		return handlers;
	}

}
}

