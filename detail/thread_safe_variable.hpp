#pragma once

#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>


namespace sam { namespace detail
{

	template <typename Type_t>
	class ts_variable
	{
	public:
		ts_variable(const Type_t &value) noexcept;
		ts_variable(Type_t &&value) noexcept;
		ts_variable(const ts_variable &another) = delete;
		ts_variable(ts_variable &&another) = default;

		ts_variable &operator =(const ts_variable &another) = delete;
		ts_variable &operator =(ts_variable &&another) = default;
		ts_variable &operator =(const Type_t &value);
		ts_variable &operator =(Type_t &&value);

		operator Type_t();

		ts_variable<Type_t> &operator =(const std::function<void (Type_t &)> &modifier);

		std::unique_lock<std::mutex> wait(const std::function<bool (const Type_t &value)> &predicate);

		template <typename Rep_t, typename Period_t>
		std::unique_lock<std::mutex> wait_for(bool &happened, const std::chrono::duration<Rep_t, Period_t> &timeout_duration, const std::function<bool (const Type_t &value)> &predicate);

	private:
		Type_t _value;
		std::mutex _mutex;
		std::condition_variable _ready;
	};

}
}


namespace sam { namespace detail
{

	template <typename Type_t>
	ts_variable<Type_t>::ts_variable(const Type_t &value) noexcept
		:
		_value {value}
	{
	}


	template <typename Type_t>
	ts_variable<Type_t>::ts_variable(Type_t &&value) noexcept
		:
		_value {std::move(value)}
	{
	}


	template <typename Type_t>
	std::unique_lock<std::mutex> ts_variable<Type_t>::wait(const std::function<bool (const Type_t &)> &predicate)
	{
		std::unique_lock<std::mutex> lock {_mutex};
		_ready.wait(lock,
			[&predicate, this]
			{
				return predicate(_value);
			});

		return std::move(lock);
	}


    template <typename Type_t>
	template <typename Rep_t, typename Period_t>
	std::unique_lock<std::mutex> ts_variable<Type_t>::wait_for(bool &happened, const std::chrono::duration<Rep_t, Period_t> &timeout_duration, const std::function<bool (const Type_t &value)> &predicate)
	{
		std::unique_lock<std::mutex> lock {_mutex};
		happened = _ready.wait_for(lock, timeout_duration,
			[&predicate, this]
			{
				return predicate(_value);
			});

		return std::move(lock);
	}


	template <typename Type_t>
	ts_variable<Type_t>::operator Type_t()
	{
		std::lock_guard<std::mutex> guard {_mutex};

		return _value;
	}


	template <typename Type_t>
	ts_variable<Type_t> &ts_variable<Type_t>::operator =(const Type_t &value)
	{
		{
			std::lock_guard<std::mutex> guard {_mutex};
			_value = value;
		}

		_ready.notify_one();

		return *this;
	}


	template <typename Type_t>
	ts_variable<Type_t> &ts_variable<Type_t>::operator =(Type_t &&value)
	{
		{
			std::lock_guard<std::mutex> guard {_mutex};
			_value = std::move(value);
		}

		_ready.notify_one();

		return *this;
	}


	template <typename Type_t>
	ts_variable<Type_t> &ts_variable<Type_t>::operator =(const std::function<void (Type_t &)> &modifier)
	{
		{
			std::lock_guard<std::mutex> guard {_mutex};
			modifier(_value);
		}

		_ready.notify_one();

		return *this;
	}

}
}
