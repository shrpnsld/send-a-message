//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#include "message.h"


namespace sam { namespace detail
{

	message::message(signature_t signature) :
		_signature(signature)
	{
	}


	message::~message()
	{
	}


	const signature_t message::signature() const
	{
		return _signature;
	}

}
}

