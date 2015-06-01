#include "handler.h"


namespace tfm
{

	//
	// Definitions

	super_handler::super_handler(const std::type_index &signature)
		: _signature(signature)
	{
	}


	const std::type_index &super_handler::signature() const
	{
		return _signature;
	}

}

