#pragma once

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <cassert>


namespace sam
{

	namespace details
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
			std::mutex _head_mutex;
			std::mutex _tail_mutex;
			std::condition_variable _is_empty;
		};


		template <typename Type_t>
		queue<Type_t>::queue()
			: _head(new node_t), _tail(_head.get())
		{
		}


		template <typename Type_t>
		queue<Type_t>::queue(queue &&)
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
			std::lock_guard<std::mutex> lock_guard(_head_mutex);
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
			std::lock_guard<std::mutex> lock_guard(_head_mutex);
			std::unique_lock<std::mutex> lock(_tail_mutex);
			_is_empty.wait(lock, [this]{return _head.get() != _tail;});
			lock.unlock();

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

