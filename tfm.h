#pragma once

#include <typeindex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <memory>

#include "thread.h"
#include "message.h"
#include "handler.h"


namespace tfm
{

	//
	// Declarations

	thread &thread_for_handle(std::thread::native_handle_type thread);

	template <typename ...Arguments_t>
	void send(thread &thread, Arguments_t &&...arguments);


	class receive
	{
	public:
		receive(std::initializer_list<super_handler *> handlers);

		void dispatch(std::unique_ptr<super_message> message) const;

	private:
		std::unordered_map<std::type_index, std::unique_ptr<super_handler>> _handlers;
	};



	//
	// Definitions

	template <typename ...Arguments_t>
	void send(thread &thread, Arguments_t &&...arguments)
	{
		std::shared_ptr<super_message> message_ptr(new message<Arguments_t...>(std::forward<Arguments_t>(arguments)...));
		thread.message_queue().push(message_ptr);
	}

}

