#pragma once

#include <thread>
#include <memory>
#include <utility>

#include "detail/message.h"
#include "detail/message_queue.h"


namespace sam
{

	class mailbox
	{
	public:
		mailbox(const std::thread &thread) noexcept;

		template <typename ...Arguments_t>
		void send(Arguments_t &&...arguments);

	private:
		detail::msgqueue_t &_message_queue;
	};

}


namespace sam
{

	template <typename ...Arguments_t>
	void mailbox::send(Arguments_t &&...arguments)
	{
		std::unique_ptr<detail::message, std::function<void (void *)>> message_ptr {detail::make_message(std::forward<Arguments_t>(arguments)...)};
		_message_queue.push(std::move(message_ptr));
	}

}

