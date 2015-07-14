#pragma once

#include <typeindex>
#include <type_traits>


namespace sam
{

	namespace details
	{

		//
		// Declarations

		typedef std::type_index signature_t;

		template <typename ...Types_t>
		struct pack_t
		{
		};


		template <typename ...Types_t>
		signature_t new_signature();



		//
		// Definitions

		template <typename ...Types_t>
		signature_t new_signature()
		{
			return signature_t(typeid(pack_t<typename std::decay<Types_t>::type...>));
		}

	}

}

