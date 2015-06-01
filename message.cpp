#include "message.h"


namespace tfm
{

	//
	// Definitions

	super_message::super_message(const std::type_index &signature)
		: _signature(signature)
	{
	}


	const std::type_index &super_message::signature() const
	{
		return _signature;
	}

}

