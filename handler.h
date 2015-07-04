#pragma once

#include <functional>
#include <memory>

#include "signature.h"
#include "apply.h"


namespace sam
{

	//
	// Declarations

	enum hretval_t
	{
		CONTINUE = 1,
		STOP = -1
	};



	namespace details
	{

		//
		// Declarations

		class super_handler
		{
		public:
			super_handler(const signature_t &signature);
			virtual ~super_handler() = 0;

			const signature_t &signature() const;

			virtual hretval_t do_call(void *context) = 0;

		private:
			signature_t _signature;
		};



		template <typename>
		class handler;


		template <typename ...Arguments_t>
		class handler<hretval_t (Arguments_t...)>
			: public super_handler
		{
		public:
			handler(std::function<hretval_t (Arguments_t...)> callable);

			virtual hretval_t do_call(void *context) override;

		private:
			std::function<hretval_t (Arguments_t...)> _callable;
		};


		template <typename ...Arguments_t>
		class handler<void (Arguments_t...)>
			: public super_handler
		{
		public:
			handler(std::function<void (Arguments_t...)> callable);

			virtual hretval_t do_call(void *context) override;

		private:
			std::function<void (Arguments_t...)> _callable;
		};


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(std::function<hretval_t (Arguments_t...)> callable);


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(hretval_t (*function_pointer)(Arguments_t...));


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(std::function<void (Arguments_t...)> callable);


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(void (*function_pointer)(Arguments_t...));



		//
		// Definitions

		template <typename ...Arguments_t>
		handler<hretval_t (Arguments_t...)>::handler(std::function<hretval_t (Arguments_t...)> callable)
			: super_handler(new_signature<Arguments_t...>()), _callable(callable)
		{
		}


		template <typename ...Arguments_t>
		hretval_t handler<hretval_t (Arguments_t...)>::do_call(void *context)
		{
			auto arguments = *reinterpret_cast<std::tuple<Arguments_t...> *>(context);
			return apply(_callable, arguments);
		}



		template <typename ...Arguments_t>
		handler<void (Arguments_t...)>::handler(std::function<void (Arguments_t...)> callable)
			: super_handler(new_signature<Arguments_t...>()), _callable(callable)
		{
		}


		template <typename ...Arguments_t>
		hretval_t handler<void (Arguments_t...)>::do_call(void *context)
		{
			auto arguments = *reinterpret_cast<std::tuple<Arguments_t...> *>(context);
			apply(_callable, arguments);
			return CONTINUE;
		}



		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(std::function<hretval_t (Arguments_t...)> callable)
		{
			return std::shared_ptr<super_handler>(new handler<hretval_t (Arguments_t...)>(callable));
		}


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(hretval_t (*function_pointer)(Arguments_t...))
		{
			return new_shared_handler(std::function<hretval_t (Arguments_t...)>(function_pointer));
		}


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(std::function<void (Arguments_t...)> callable)
		{
			return std::shared_ptr<super_handler>(new handler<void (Arguments_t...)>(callable));
		}


		template <typename ...Arguments_t>
		std::shared_ptr<super_handler> new_shared_handler(void (*function_pointer)(Arguments_t...))
		{
			return new_shared_handler(std::function<void (Arguments_t...)>(function_pointer));
		}

	}

}

