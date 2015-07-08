#pragma once

#include <functional>


namespace sam
{

	namespace details
	{

		class call_finally
		{
		public:
			call_finally(std::function<void ()> callable)
				: _callable(callable)
			{
			}


			~call_finally()
			{
				_callable();
			}

		private:
			std::function<void ()> _callable;
		};

	}

}

