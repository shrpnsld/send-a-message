//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <typeindex>


namespace sam { namespace detail
{

	//
	// Module public

	using signature_t = std::type_index;


	template <typename ...Types_t>
	signature_t make_signature();

	template <typename Callable_t>
	signature_t make_signature(const Callable_t &);

	template <typename Callable_t, typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (Callable_t::*)(Arguments_t...) const);

	template <typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (*)(Arguments_t...));


	//
	// Module private

	template <typename ...Types_t>
	struct pack_t
	{
	};

}
}


namespace sam { namespace detail
{

	template <typename ...Types_t>
	signature_t make_signature()
	{
		return signature_t(typeid(pack_t<typename std::decay<Types_t>::type...>));
	}


	template <typename Callable_t, typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (Callable_t::*)(Arguments_t...) const)
	{
		return make_signature<Arguments_t...>();
	}


	template <typename Callable_t>
	signature_t make_signature(const Callable_t &)
	{
		return make_signature(&Callable_t::operator ());
	}


	template <typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (*)(Arguments_t...))
	{
		return make_signature<Arguments_t...>();
	}

}
}

