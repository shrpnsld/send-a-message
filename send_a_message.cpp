#include <map>
#include <mutex>
#include <cassert>

#include "send_a_message.h"


namespace sam
{

	namespace details
	{

		//
		// Definitions

		int register_handler(std::unordered_map<signature_t, std::shared_ptr<handler>> &handlers, std::shared_ptr<handler> handler)
		{
			handlers.insert(std::make_pair(handler->signature(), handler));
			return 0;
		}


		ctlcode_t default_control_code_handler(ctlcode_t control_codee)
		{
			return control_codee;
		}


		ctlcode_t dispatch_message(const std::unordered_map<signature_t, std::shared_ptr<handler>> &handlers, std::shared_ptr<message> message_ptr)
		{
			auto iterator = handlers.find(message_ptr->signature());
			assert(iterator != handlers.end()); // if fails - handler for message signature not found

			std::shared_ptr<handler> handler_ptr = iterator->second;
			return handler_ptr->do_call(message_ptr->data());
		}

	}

}

