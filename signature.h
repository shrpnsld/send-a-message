#pragma once

#include <typeindex>
#include <type_traits>


namespace sam
{

	namespace details
	{

		//
		// Declarations

		template <typename ...Types_t>
		struct signature_t
		{
		};


		template <typename ...Types_t>
		std::type_index new_signature();



		//
		// Definitions

		template <typename ...Types_t>
		std::type_index new_signature()
		{
			return std::type_index(typeid(signature_t<typename std::remove_reference<Types_t>::type...>));
		}

	}

}

