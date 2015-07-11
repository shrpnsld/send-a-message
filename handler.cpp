#include "handler.h"


namespace sam
{

	namespace details
	{

		//
		// Definitions

		handler::handler(const std::type_index &signature)
			: _signature(signature)
		{
		}


		handler::~handler()
		{
		}


		const std::type_index &handler::signature() const
		{
			return _signature;
		}

	}

}

