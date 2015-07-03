#pragma once

#include <thread>
#include <utility>

#include "message.h"
#include "queue.h"


namespace sam
{

	class thread
	{
	public:
		thread();
		thread(thread &&another);

		queue<message> &message_queue();

	private:
		queue<message> _message_queue;
	};

}

