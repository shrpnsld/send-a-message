#pragma once

#include <tuple>
#include <utility>
#include <memory>
#include <functional>

#include "signature.h"


namespace sam { namespace detail
{

	//
	// Module public

	class alignas(signature_t) message
	{
	public:
		const signature_t signature() const noexcept;
		void *data() noexcept;

	private:
		signature_t _signature;
		void *_data; // address of this field is used as an address of concrete_message::_data. this pointer is never dereferenced.
	};


	template <typename ...Types_t>
	using concrete_message_deleter_t = void (*)(void *);

	template <typename ...Types_t>
	std::unique_ptr<message, concrete_message_deleter_t<Types_t...>> make_message(Types_t &&...arguments);


	//
	// Module private

	template <typename ...Types_t>
	class alignas(signature_t) concrete_message
	{
	public:
		concrete_message(Types_t &&...arguments) noexcept;

	private:
		signature_t _signature;
		std::tuple<Types_t...> _data;
	};


	template <typename ...Types_t>
	void concrete_message_deleter(void *pointer);

}
}


namespace sam { namespace detail
{

	template <typename ...Types_t>
	std::unique_ptr<message, concrete_message_deleter_t<Types_t...>> make_message(Types_t &&...arguments)
	{
		auto *concrete_message_pointer {new concrete_message<Types_t...> {std::forward<Types_t>(arguments)...}};
		auto deleter {concrete_message_deleter<Types_t...>};

		return {reinterpret_cast<message *>(concrete_message_pointer), deleter};
	}


	template <typename ...Types_t>
	concrete_message<Types_t...>::concrete_message(Types_t &&...arguments) noexcept
		:
		_signature {make_signature<Types_t...>()},
		_data {std::forward<Types_t>(arguments)...}
	{
	}


	template <typename ...Types_t>
	void concrete_message_deleter(void *pointer)
	{
		concrete_message<Types_t...> *concrete_message_pointer {reinterpret_cast<concrete_message<Types_t...> *>(pointer)};
		delete concrete_message_pointer;
	}

}
}

