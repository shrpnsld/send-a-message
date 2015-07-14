//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <typeindex>
#include <type_traits>


namespace sam
{

	//
	// Private
	//


	//
	// Declarations

	namespace details
	{

		typedef std::type_index signature_t;

		template <typename ...Types_t>
		struct pack_t
		{
		};

		template <typename ...Types_t>
		signature_t make_signature();

	}


	//
	// Definitions

	namespace details
	{

		template <typename ...Types_t>
		signature_t make_signature()
		{
			return signature_t(typeid(pack_t<typename std::decay<Types_t>::type...>));
		}

	}

}

