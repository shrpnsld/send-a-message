//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once


namespace sam
{

	//
	// Private
	//


	//
	// Declarations

	namespace details
	{

		template <std::size_t ...>
		struct sequence
		{
		};


		template <size_t Number, typename Sequence_t, typename ...Types_t>
		struct make_sequence_impl;


		template <size_t Number, size_t ...Numbers, typename Type_t, typename ...Types_t>
		struct make_sequence_impl<Number, sequence<Numbers...>, Type_t, Types_t...>
		{
			typedef typename make_sequence_impl<Number + 1, sequence<Numbers..., Number>, Types_t...>::type type;
		};


		template <size_t Number, size_t ...Numbers>
		struct make_sequence_impl<Number, sequence<Numbers...>>
		{
			typedef sequence<Numbers...> type;
		};


		template <typename ...Types_t>
		struct make_sequence
		{
			typedef typename make_sequence_impl<0, sequence<>, Types_t...>::type type;
		};


		template <typename Callable_t, typename Return_t, typename ...Arguments_t, size_t ...Indices>
		Return_t apply_impl(Callable_t &&callable, const std::tuple<Arguments_t...> &arguments, sequence<Indices...>);

		template <typename Callable_t, typename Return_t, typename ...Arguments_t>
		Return_t apply(Callable_t &callable, const std::tuple<Arguments_t...> &arguments);

	}


	//
	// Definitions

	namespace details
	{

		template <typename Callable_t, typename Return_t, typename ...Arguments_t, size_t ...Indices>
		Return_t apply_impl(Callable_t &&callable, const std::tuple<Arguments_t...> &arguments, sequence<Indices...>)
		{
			return callable(std::get<Indices>(arguments)...);
		}


		template <typename Callable_t, typename Return_t, typename ...Arguments_t>
		Return_t apply(Callable_t &callable, const std::tuple<Arguments_t...> &arguments)
		{
			return apply_impl<Callable_t, Return_t, Arguments_t...>(std::forward<Callable_t>(callable), arguments, typename make_sequence<Arguments_t...>::type());
		}

	}

}

