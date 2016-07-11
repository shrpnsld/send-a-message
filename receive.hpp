#pragma once

#include <chrono>
#include <thread>
#include <memory>
#include <utility>

#include "flags.hpp"
#include "detail/handler.hpp"
#include "detail/handler_registry.hpp"
#include "detail/message.hpp"
#include "detail/message_queue.hpp"


namespace sam
{

	template <typename ...Callables_t>
	void receive(Callables_t &&...callables);

	template <typename Rep_t, typename Period_t, typename ...Callables_t>
	void receive_for(const std::chrono::duration<Rep_t, Period_t> &timeout, Callables_t &&...callables);

}


namespace sam
{

	template <typename ...Callables_t>
	void receive(Callables_t &&...callables)
	{
		detail::handlers_t handlers {detail::register_handlers(std::forward<Callables_t>(callables)...)};
		detail::msgqueue_t &message_queue {detail::message_queue_for_thread(std::this_thread::get_id())};
		for (;;)
		{
			std::shared_ptr<detail::message> message_ptr {message_queue.wait_and_pop()};
			ctlcode_t control_code {detail::dispatch_message(handlers, message_ptr)};
			if (control_code == STOP)
			{
				break;
			}
		}
	}


	template <typename Rep_t, typename Period_t, typename ...Callables_t>
	void receive_for(const std::chrono::duration<Rep_t, Period_t> &timeout, Callables_t &&...callables)
	{
		detail::handlers_t handlers {detail::register_handlers_with_timeout(std::forward<Callables_t>(callables)...)};
		detail::msgqueue_t &message_queue {detail::message_queue_for_thread(std::this_thread::get_id())};
		for (;;)
		{
			std::shared_ptr<detail::message> message_ptr {message_queue.wait_for_and_pop(timeout)};
			if (message_ptr == nullptr)
			{
				message_ptr = detail::make_message(timeout_error());
			}

			ctlcode_t control_code {detail::dispatch_message(handlers, message_ptr)};
			if (control_code == STOP)
			{
				break;
			}
		}
	}

}

