//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#include "message.h"


namespace sam { namespace detail
{

	const signature_t message::signature() const
	{
		return _signature;
	}


	void *message::data()
	{
		return reinterpret_cast<void *>(&_data);
	}

}
}

