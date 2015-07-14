//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <utility>
#include <memory>

#include "signature.h"


namespace sam
{

	//
	// Private
	//


	//
	// Declarations

	namespace details
	{

		class message
		{
		public:
			message(signature_t signature);
			virtual ~message() = 0;

			const signature_t signature() const;

			virtual void *data() = 0;

		private:
			signature_t _signature;
		};


		template <typename ...Types_t>
		class concrete_message
			: public message
		{
		public:
			template <typename ...DataTypes_t>
			concrete_message(DataTypes_t &&...arguments);

			virtual void *data() override;

		private:
			std::tuple<Types_t...> _data;
		};


		template <typename ...Types_t>
		std::shared_ptr<message> make_shared_message(Types_t &&...arguments);

	}


	//
	// Definitions

	namespace details
	{

		template <typename ...Types_t>
		template <typename ...DataTypes_t>
		concrete_message<Types_t...>::concrete_message(DataTypes_t &&...arguments) :
			message{make_signature<Types_t...>()},
			_data{std::forward<DataTypes_t>(arguments)...}
		{
		}


		template <typename ...Types_t>
		void *concrete_message<Types_t...>::data()
		{
			return reinterpret_cast<void *>(&_data);
		}


		template <typename ...Types_t>
		std::shared_ptr<message> make_shared_message(Types_t &&...arguments)
		{
			return std::shared_ptr<message>{new concrete_message<Types_t...>(std::forward<Types_t>(arguments)...)};
		}

	}

}

