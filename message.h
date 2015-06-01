#pragma once

#include <typeindex>
#include <functional>
#include <utility>


namespace tfm
{

	//
	// Declarations

	class super_message
	{
	public:
		super_message(const std::type_index &signature);

		const std::type_index &signature() const;

		virtual void *data() = 0;

	private:
		std::type_index _signature;
	};


	template <typename ...Types_t>
	class message
		: public super_message
	{
	public:
		template <typename ...DataTypes_t>
		message(DataTypes_t &&...values);

		virtual void *data() override;

	private:
		std::tuple<Types_t...> _data;
	};



	//
	// Definitions

	template <typename ...Types_t>
	template <typename ...DataTypes_t>
	message<Types_t...>::message(DataTypes_t &&...values)
		: super_message(std::type_index(typeid(std::function<void (Types_t...)>))), _data(std::forward<DataTypes_t>(values)...)
	{
	}


	template <typename ...Types_t>
	void *message<Types_t...>::data()
	{
		return &_data;
	}

}

