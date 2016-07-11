#pragma once

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "thread_safe_variable.hpp"


namespace sam { namespace detail
{

	//
	// Module public

	template <typename Type_t>
	class ts_queue
	{
	public:
		ts_queue();
		ts_queue(const ts_queue &another) = delete;
		ts_queue(ts_queue &&another) = default;
		~ts_queue() = default;

		ts_queue &operator =(const ts_queue &another) = delete;
		ts_queue &operator =(ts_queue &&another) = default;

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
		ts_variable<node_t *> _tail;
	};

}
}


namespace sam { namespace detail
{

	template <typename Type_t>
	ts_queue<Type_t>::ts_queue()
		:
		_head {new node_t},
		_tail {_head.get()}
	{
	}


	template <typename Type_t>
	void ts_queue<Type_t>::push(std::shared_ptr<Type_t> data)
	{
		_tail = [data](node_t *&tail)
		{
			tail->data = data;
			tail->next.reset(new node_t);
			tail = tail->next.get();
		};
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> ts_queue<Type_t>::pop()
	{
		if (_head.get() == _tail)
		{
			return {nullptr};
		}

		std::shared_ptr<Type_t> data {_head->data};
		std::unique_ptr<node_t> next {std::move(_head->next)};
		_head = std::move(next);

		return data;
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> ts_queue<Type_t>::wait_and_pop()
	{
		_tail.wait(
			[this](node_t * const &tail)
			{
				return _head.get() != tail;
			});

		std::shared_ptr<Type_t> data {_head->data};
		std::unique_ptr<node_t> next {std::move(_head->next)};
		_head = std::move(next);

		return data;
	}


	template <typename Type_t>
	template <typename Rep_t, typename Period_t>
	std::shared_ptr<Type_t> ts_queue<Type_t>::wait_for_and_pop(const std::chrono::duration<Rep_t, Period_t> &timeout_duration)
	{
		bool has_nodes;
		_tail.wait_for(has_nodes, timeout_duration,
			[this](node_t * const &tail)
			{
				return _head.get() != tail;
			});

		if (!has_nodes)
		{
			return {nullptr};
		}

		std::shared_ptr<Type_t> data {_head->data};
		std::unique_ptr<node_t> next {std::move(_head->next)};
		_head = std::move(next);

		return data;
	}

}
}

