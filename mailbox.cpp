#include "mailbox.h"


namespace sam
{

	mailbox::mailbox(const std::thread &thread) :
		_message_queue(detail::message_queue_for_thread(thread.get_id()))
	{
	}

}

