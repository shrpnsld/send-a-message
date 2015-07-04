#include "handler.h"


namespace sam
{

	namespace details
	{

		//
		// Definitions

		super_handler::super_handler(const std::type_index &signature)
			: _signature(signature)
		{
		}


		super_handler::~super_handler()
		{
		}


		const std::type_index &super_handler::signature() const
		{
			return _signature;
		}

	}

}

