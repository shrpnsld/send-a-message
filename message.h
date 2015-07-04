#pragma once

#include <utility>
#include <memory>

#include "signature.h"


namespace sam
{

	namespace details
	{

		//
		// Declarations

		class super_fields
		{
		public:
			virtual ~super_fields() = 0;

			virtual void *void_ptr() = 0;
		};


		template <typename ...Types_t>
		class fields
			: public super_fields
		{
		public:
			template <typename ...DataTypes_t>
			fields(DataTypes_t &&...arguments);

			virtual void *void_ptr() override;

		private:
			std::tuple<typename std::remove_reference<Types_t>::type...> _data;
		};


		class message
		{
		public:
			message(signature_t signature, std::shared_ptr<super_fields> data);

			const signature_t &signature() const;
			void *data();

		private:
			signature_t _signature;
			std::shared_ptr<super_fields> _data;
		};


		template <typename ...Types_t>
		message new_message(Types_t &&...arguments);



		//
		// Definitions

		template <typename ...Types_t>
		template <typename ...DataTypes_t>
		fields<Types_t...>::fields(DataTypes_t &&...arguments)
			: _data(std::forward<DataTypes_t>(arguments)...)
		{
		}


		template <typename ...Types_t>
		void *fields<Types_t...>::void_ptr()
		{
			return reinterpret_cast<void *>(&_data);
		}


		template <typename ...Types_t>
		message new_message(Types_t &&...arguments)
		{
			signature_t signature = new_signature<Types_t...>();
			std::shared_ptr<super_fields> data{new fields<Types_t...>(std::forward<Types_t>(arguments)...)};
			return message(signature, data);
		}

	}

}

