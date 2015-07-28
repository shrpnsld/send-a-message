//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <functional>
#include <memory>

#include "signature.h"
#include "apply.h"


namespace sam
{

	//
	// Public
	//


	//
	// Declarations

	enum ctlcode_t
	{
		CONTINUE = 1,
		STOP = -1
	};


	struct timeout
	{
	};



	//
	// Private
	//


	//
	// Declarations

	namespace details
	{

		class handler
		{
		public:
			handler(const signature_t &signature);
			virtual ~handler() = 0;

			const signature_t &signature() const;

			virtual ctlcode_t do_call(void *context) = 0;

		private:
			signature_t _signature;
		};



		template <typename Callable_t, typename Return_t, typename ...Arguments_t>
		class concrete_handler;


		template <typename Callable_t, typename ...Arguments_t>
		class concrete_handler<Callable_t, ctlcode_t, Arguments_t...>
			: public handler
		{
		public:
			template <typename CallableArg_t>
			concrete_handler(CallableArg_t &&callable);

			ctlcode_t do_call(void *context);

		private:
			Callable_t _callable;
		};


		template <typename Callable_t, typename ...Arguments_t>
		class concrete_handler<Callable_t, void, Arguments_t...>
			: public handler
		{
		public:
			template <typename CallableArg_t>
			concrete_handler(CallableArg_t &&callable);

			ctlcode_t do_call(void *context);

		private:
			Callable_t _callable;
		};



		template <typename Callable_t, typename Return_t, typename ...Arguments_t>
		std::shared_ptr<handler> make_handler_from_callable(Callable_t &&callable, Return_t (Callable_t::*)(Arguments_t...) const);

		template <typename Callable_t>
		std::shared_ptr<handler> make_shared_handler(Callable_t &&callable);

		template <typename Return_t, typename ...Arguments_t>
		std::shared_ptr<handler> make_shared_handler(Return_t (*function_pointer)(Arguments_t...));

	}


	//
	// Definitions

	namespace details
	{

		template <typename Callable_t, typename ...Arguments_t>
		template <typename CallableArg_t>
		concrete_handler<Callable_t, ctlcode_t, Arguments_t...>::concrete_handler(CallableArg_t &&callable) :
			handler(make_signature<Arguments_t...>()),
			_callable(std::forward<CallableArg_t>(callable))
		{
		}


		template <typename Callable_t, typename ...Arguments_t>
		ctlcode_t concrete_handler<Callable_t, ctlcode_t, Arguments_t...>::do_call(void *context)
		{
			std::tuple<Arguments_t...> &arguments = *reinterpret_cast<std::tuple<Arguments_t...> *>(context);
			return apply<Callable_t, ctlcode_t, Arguments_t...>(_callable, arguments);
		}


		template <typename Callable_t, typename ...Arguments_t>
		template <typename CallableArg_t>
		concrete_handler<Callable_t, void, Arguments_t...>::concrete_handler(CallableArg_t &&callable) :
			handler(make_signature<Arguments_t...>()),
			_callable(std::forward<CallableArg_t>(callable))
		{
		}


		template <typename Callable_t, typename ...Arguments_t>
		ctlcode_t concrete_handler<Callable_t, void, Arguments_t...>::do_call(void *context)
		{
			std::tuple<Arguments_t...> &arguments = *reinterpret_cast<std::tuple<Arguments_t...> *>(context);
			apply<Callable_t, void, Arguments_t...>(_callable, arguments);
			return CONTINUE;
		}


		template <typename Callable_t, typename Return_t, typename ...Arguments_t>
		std::shared_ptr<handler> make_handler_from_callable(Callable_t &&callable, Return_t (Callable_t::*)(Arguments_t...) const)
		{
			return std::shared_ptr<handler>(new concrete_handler<Callable_t, Return_t, Arguments_t...>(std::forward<Callable_t>(callable)));
		}


		template <typename Callable_t>
		std::shared_ptr<handler> make_shared_handler(Callable_t &&callable)
		{
			return make_handler_from_callable(std::forward<Callable_t>(callable), &Callable_t::operator ());
		}


		template <typename Return_t, typename ...Arguments_t>
		std::shared_ptr<handler> make_shared_handler(Return_t (*function_pointer)(Arguments_t...))
		{
			return std::shared_ptr<handler>(new concrete_handler<Return_t (*)(Arguments_t...), Return_t, Arguments_t...>(function_pointer));
		}

	}

}

