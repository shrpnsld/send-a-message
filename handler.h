#pragma once

#include <typeindex>
#include <functional>

#include "apply.h"


namespace tfm
{

	//
	// Declarations

	class super_handler
	{
	public:
		super_handler(const std::type_index &signature);

		const std::type_index &signature() const;

		virtual void do_call(const void *context) const = 0;

	private:
		std::type_index _signature;
	};


	template <typename ...Arguments_t>
	class handler
		: public super_handler
	{
	public:
		handler(const std::function<void (Arguments_t...)> &callable);

		virtual void do_call(const void *context) const override;

	private:
		std::function<void (Arguments_t...)> _callable;
	};


	template <typename ...Arguments_t>
	handler<Arguments_t...> *new_handler(void (*function_pointer)(Arguments_t...));



	//
	// Definitions

	template <typename ...Arguments_t>
	handler<Arguments_t...>::handler(const std::function<void (Arguments_t...)> &callable)
		: super_handler(std::type_index(typeid(std::function<void (Arguments_t...)>))), _callable(callable)
	{
	}


	template <typename ...Arguments_t>
	void handler<Arguments_t...>::do_call(const void *context) const
	{
		auto arguments = *reinterpret_cast<const std::tuple<Arguments_t...> *>(context);
		apply(_callable, arguments);
	}


	template <typename ...Arguments_t>
	handler<Arguments_t...> *new_handler(void (*function_pointer)(Arguments_t...))
	{
		return new handler<Arguments_t...>(function_pointer);
	}

}

