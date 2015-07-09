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
			return std::type_index(typeid(pack_t<typename std::remove_reference<Types_t>::type...>));
		}

	}

}

