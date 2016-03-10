#pragma once

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "ts_var.h"


namespace sam { namespace detail
{

	//
	// Module public

	template <typename Type_t>
	class queue
	{
	public:
		queue();
		queue(const queue &another) = delete;
		queue(queue &&another) = default;
		~queue();

		queue &operator =(const queue &another) = delete;
		queue &operator =(queue &&another) = default;

		void push(std::shared_ptr<Type_t> data);
		std::shared_ptr<Type_t> pop();
		std::shared_ptr<Type_t> wait_and_pop();

		template <typename Rep_t, typename Period_t>
		std::shared_ptr<Type_t> wait_for_and_pop(const std::chrono::duration<Rep_t, Period_t> &timeout_duration);

	private:
		struct node_t
		{
			std::shared_ptr<Type_t> data;
			std::unique_ptr<node_t> next;
		};

		std::unique_ptr<node_t> _head;
		ts_var<node_t *> _tail;
	};

}
}


namespace sam { namespace detail
{

	template <typename Type_t>
	queue<Type_t>::queue() :
		_head(new node_t),
		_tail(_head.get())
	{
	}


	template <typename Type_t>
	queue<Type_t>::~queue()
	{
	}


	template <typename Type_t>
	void queue<Type_t>::push(std::shared_ptr<Type_t> data)
	{
		_tail = [data](node_t *&tail)
		{
			tail->data = data;
			tail->next.reset(new node_t);
			tail = tail->next.get();
		};
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> queue<Type_t>::pop()
	{
		if (_head.get() == _tail)
		{
			return std::shared_ptr<Type_t>(nullptr);
		}

		std::shared_ptr<Type_t> data = _head->data;
		std::unique_ptr<node_t> next = std::move(_head->next);
		_head = std::move(next);

		return data;
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> queue<Type_t>::wait_and_pop()
	{
		_tail.wait(
			[this](node_t * const &tail)
			{
				return _head.get() != tail;
			});

		std::shared_ptr<Type_t> data = _head->data;
		std::unique_ptr<node_t> next = std::move(_head->next);
		_head = std::move(next);

		return data;
	}


	template <typename Type_t>
	template <typename Rep_t, typename Period_t>
	std::shared_ptr<Type_t> queue<Type_t>::wait_for_and_pop(const std::chrono::duration<Rep_t, Period_t> &timeout_duration)
	{
		bool has_messages;
		_tail.wait_for(has_messages, timeout_duration,
			[this](node_t * const &tail)
			{
				return _head.get() != tail;
			});

		if (!has_messages)
		{
			return std::shared_ptr<Type_t>(nullptr);
		}

		std::shared_ptr<Type_t> data = _head->data;
		std::unique_ptr<node_t> next = std::move(_head->next);
		_head = std::move(next);

		return data;
	}

}
}

