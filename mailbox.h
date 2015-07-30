//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

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
		mailbox(const std::thread &thread);

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
		std::shared_ptr<detail::message> message_ptr = detail::make_shared_message(std::forward<Arguments_t>(arguments)...);
		_message_queue.push(message_ptr);
	}

}

