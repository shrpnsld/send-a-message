#pragma once

#include <typeindex>


namespace sam { namespace detail
{

	//
	// Module public

	using signature_t = std::type_index;


	template <typename ...Types_t>
	signature_t make_signature() noexcept;

	template <typename Callable_t>
	signature_t make_signature(const Callable_t &) noexcept;

	template <typename Callable_t, typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (Callable_t::*)(Arguments_t...) const) noexcept;

	template <typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (*)(Arguments_t...)) noexcept;


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
	signature_t make_signature() noexcept
	{
		return signature_t {typeid(pack_t<typename std::decay<Types_t>::type...>)};
	}


	template <typename Callable_t, typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (Callable_t::*)(Arguments_t...) const) noexcept
	{
		return make_signature<Arguments_t...>();
	}


	template <typename Callable_t>
	signature_t make_signature(const Callable_t &) noexcept
	{
		return make_signature(&Callable_t::operator ());
	}


	template <typename Return_t, typename ...Arguments_t>
	signature_t make_signature(Return_t (*)(Arguments_t...)) noexcept
	{
		return make_signature<Arguments_t...>();
	}

}
}

