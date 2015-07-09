#pragma once


namespace sam
{

	namespace details
	{

		//
		// Declarations

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


		template <typename Return_t, typename ...Arguments_t, size_t ...Indices>
		Return_t apply_impl(const std::function<Return_t (Arguments_t...)> &callable, const std::tuple<Arguments_t...> &arguments, sequence<Indices...>);

		template <typename Return_t, typename ...Arguments_t>
		Return_t apply(const std::function<Return_t (Arguments_t...)> &callable, const std::tuple<Arguments_t...> &arguments);


		//
		// Definitions

		template <typename Return_t, typename ...Arguments_t, size_t ...Indices>
		Return_t apply_impl(const std::function<Return_t (Arguments_t...)> &callable, const std::tuple<Arguments_t...> &arguments, sequence<Indices...>)
		{
			return callable(std::get<Indices>(arguments)...);
		}


		template <typename Return_t, typename ...Arguments_t>
		Return_t apply(const std::function<Return_t (Arguments_t...)> &callable, const std::tuple<Arguments_t...> &arguments)
		{
			return apply_impl(callable, arguments, typename make_sequence<Arguments_t...>::type());
		}

	}

}

