#pragma once

#include <functional>
#include <memory>

#include "signature.h"
#include "apply.h"


namespace sam
{

	//
	// Declarations

	enum ctlcode_t
	{
		CONTINUE = 1,
		STOP = -1
	};



	namespace details
	{

		//
		// Declarations

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



		template <typename>
		class concrete_handler;


		template <typename ...Arguments_t>
		class concrete_handler<ctlcode_t (Arguments_t...)>
			: public handler
		{
		public:
			concrete_handler(std::function<ctlcode_t (Arguments_t...)> function);

			virtual ctlcode_t do_call(void *context) override;

		private:
			std::function<ctlcode_t (Arguments_t...)> _function;
		};


		template <typename ...Arguments_t>
		class concrete_handler<void (Arguments_t...)>
			: public handler
		{
		public:
			concrete_handler(std::function<void (Arguments_t...)> function);

			virtual ctlcode_t do_call(void *context) override;

		private:
			std::function<void (Arguments_t...)> _function;
		};


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(std::function<ctlcode_t (Arguments_t...)> function);


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(ctlcode_t (*function_pointer)(Arguments_t...));


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(std::function<void (Arguments_t...)> function);


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(void (*function_pointer)(Arguments_t...));



		//
		// Definitions

		template <typename ...Arguments_t>
		concrete_handler<ctlcode_t (Arguments_t...)>::concrete_handler(std::function<ctlcode_t (Arguments_t...)> function) :
			handler{new_signature<Arguments_t...>()},
			_function{function}
		{
		}


		template <typename ...Arguments_t>
		ctlcode_t concrete_handler<ctlcode_t (Arguments_t...)>::do_call(void *context)
		{
			std::tuple<Arguments_t...> arguments{*reinterpret_cast<std::tuple<Arguments_t...> *>(context)};
			return apply(_function, arguments);
		}



		template <typename ...Arguments_t>
		concrete_handler<void (Arguments_t...)>::concrete_handler(std::function<void (Arguments_t...)> function) :
			handler{new_signature<Arguments_t...>()},
			_function{function}
		{
		}


		template <typename ...Arguments_t>
		ctlcode_t concrete_handler<void (Arguments_t...)>::do_call(void *context)
		{
			std::tuple<Arguments_t...> arguments{*reinterpret_cast<std::tuple<Arguments_t...> *>(context)};
			apply(_function, arguments);
			return CONTINUE;
		}



		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(std::function<ctlcode_t (Arguments_t...)> function)
		{
			return std::shared_ptr<handler>{new concrete_handler<ctlcode_t (Arguments_t...)>(function)};
		}


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(ctlcode_t (*function_pointer)(Arguments_t...))
		{
			return new_shared_handler(std::function<ctlcode_t (Arguments_t...)>(function_pointer));
		}


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(std::function<void (Arguments_t...)> function)
		{
			return std::shared_ptr<handler>{new concrete_handler<void (Arguments_t...)>(function)};
		}


		template <typename ...Arguments_t>
		std::shared_ptr<handler> new_shared_handler(void (*function_pointer)(Arguments_t...))
		{
			return new_shared_handler(std::function<void (Arguments_t...)>(function_pointer));
		}

	}

}

