//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#pragma once

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <chrono>


namespace sam { namespace detail
{

	template <typename Type_t>
	class queue
	{
	public:
		queue();
		queue(queue &&another);
		~queue();

		void push(std::shared_ptr<Type_t> data);
		std::shared_ptr<Type_t> pop();
		std::shared_ptr<Type_t> wait_and_pop();

		template <typename Rep, typename Period>
		std::shared_ptr<Type_t> wait_for_and_pop(const std::chrono::duration<Rep, Period> &timeout_duration);

	private:
		struct node_t
		{
			std::shared_ptr<Type_t> data;
			std::unique_ptr<node_t> next;
		};

		node_t *_synchronized_get_tail();
		std::shared_ptr<Type_t> _pop();

		std::unique_ptr<node_t> _head;
		node_t *_tail;
		std::mutex _tail_mutex;
		std::condition_variable _is_empty;
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
	queue<Type_t>::queue(queue &&another) :
		_head(std::move(another._head)),
		_tail(another._tail)
	{
	}


	template <typename Type_t>
	queue<Type_t>::~queue()
	{
	}


	template <typename Type_t>
	void queue<Type_t>::push(std::shared_ptr<Type_t> data)
	{
		{
			std::lock_guard<std::mutex> lock_guard(_tail_mutex);

			_tail->data = data;
			_tail->next.reset(new node_t);
			_tail = _tail->next.get();
		}

		_is_empty.notify_one();
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> queue<Type_t>::pop()
	{
		if (_head.get() == _synchronized_get_tail())
		{
			return std::shared_ptr<Type_t>(nullptr);
		}

		return _pop();
	}


	template <typename Type_t>
	typename queue<Type_t>::node_t *queue<Type_t>::_synchronized_get_tail()
	{
		std::lock_guard<std::mutex> lock_guard(_tail_mutex);
		return _tail;
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> queue<Type_t>::wait_and_pop()
	{
		std::unique_lock<std::mutex> lock(_tail_mutex);
		_is_empty.wait(lock, [this]{return _head.get() != _tail;});
		lock.unlock();

		return _pop();
	}


	template <typename Type_t>
	template <typename Rep, typename Period>
	std::shared_ptr<Type_t> queue<Type_t>::wait_for_and_pop(const std::chrono::duration<Rep, Period> &timeout_duration)
	{
		std::unique_lock<std::mutex> lock(_tail_mutex);
		bool has_messages = _is_empty.wait_for(lock, timeout_duration, [this]{return _head.get() != _tail;});
		lock.unlock();

		if (!has_messages)
		{
			return std::shared_ptr<Type_t>(nullptr);
		}

		return _pop();
	}


	template <typename Type_t>
	std::shared_ptr<Type_t> queue<Type_t>::_pop()
	{
		std::shared_ptr<Type_t> data = _head->data;
		std::unique_ptr<node_t> next = std::move(_head->next);
		_head = std::move(next);

		return data;
	}

}
}

