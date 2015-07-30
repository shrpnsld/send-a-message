//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <chrono>
#include <thread>
#include <memory>
#include <utility>

#include "flags.h"
#include "detail/handler.h"
#include "detail/handler_registry.h"
#include "detail/message.h"
#include "detail/message_queue.h"


namespace sam
{
	template <typename ...Callables_t>
	void receive(Callables_t &&...callables);

	template <typename Rep, typename Period, typename ...Callables_t>
	void receive_for(const std::chrono::duration<Rep, Period> &timeout, Callables_t &&...callables);

}


namespace sam
{

	template <typename ...Callables_t>
	void receive(Callables_t &&...callables)
	{
		detail::handlers_t handlers = detail::register_handlers(std::forward<Callables_t>(callables)...);
		detail::msgqueue_t &message_queue = detail::message_queue_for_thread(std::this_thread::get_id());
		for (;;)
		{
			std::shared_ptr<detail::message> message_ptr = message_queue.wait_and_pop();
			ctlcode_t control_code = detail::dispatch_message(handlers, message_ptr);
			if (control_code == STOP)
			{
				break;
			}
		}
	}


	template <typename Rep, typename Period, typename ...Callables_t>
	void receive_for(const std::chrono::duration<Rep, Period> &timeout, Callables_t &&...callables)
	{
		detail::handlers_t handlers = detail::register_handlers_with_timeout(std::forward<Callables_t>(callables)...);
		detail::msgqueue_t &message_queue = detail::message_queue_for_thread(std::this_thread::get_id());
		for (;;)
		{
			std::shared_ptr<detail::message> message_ptr = message_queue.wait_for_and_pop(timeout);
			if (message_ptr == nullptr)
			{
				message_ptr = detail::make_shared_message(timeout_error());
			}

			ctlcode_t control_code = detail::dispatch_message(handlers, message_ptr);
			if (control_code == STOP)
			{
				break;
			}
		}
	}

}

