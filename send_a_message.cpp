//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#include <map>
#include <mutex>
#include <cassert>

#include "send_a_message.h"


namespace sam
{

	//
	// Public
	//


	//
	// Definitions

	mailbox::mailbox(const std::thread &thread) :
		_message_queue{details::message_queue_for_thread(thread.get_id())}
	{
	}



	//
	// Private
	//


	//
	// Definitions

	namespace details
	{

		int register_handler(handlers_t &handlers, std::shared_ptr<handler> handler)
		{
			handlers.insert(std::make_pair(handler->signature(), handler));
			return 0;
		}


		ctlcode_t default_control_code_handler(ctlcode_t control_code)
		{
			return control_code;
		}


		ctlcode_t default_timeout_handler(timeout)
		{
			std::this_thread::yield();
			return CONTINUE;
		}


		ctlcode_t dispatch_message(const handlers_t &handlers, std::shared_ptr<message> message_ptr)
		{
			auto iterator = handlers.find(message_ptr->signature());
			assert(iterator != handlers.end()); // if fails - handler for message signature not found

			std::shared_ptr<handler> handler_ptr{iterator->second};
			return handler_ptr->do_call(message_ptr->data());
		}

	}

}

