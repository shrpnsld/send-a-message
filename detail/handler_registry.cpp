//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#include <thread>
#include <cassert>

#include "handler_registry.h"


namespace sam { namespace detail
{

	ctlcode_t default_control_code_handler(ctlcode_t control_code)
	{
		return control_code;
	}


	ctlcode_t default_timeout_handler(timeout_error)
	{
		std::this_thread::yield();
		return CONTINUE;
	}


	ctlcode_t dispatch_message(const handlers_t &handlers, std::shared_ptr<message> message_ptr)
	{
		auto iterator = handlers.find(message_ptr->signature());
		assert(iterator != handlers.end()); // if fails - handler for message signature not found

		const handler_t &handler = iterator->second;
		return handler(message_ptr->data());
	}

}
}

