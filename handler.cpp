//
// This file is a part of 'send-a-message' project
// See more at https://github.com/shrpnsld/send-a-message
//

#include "handler.h"


namespace sam
{

	//
	// Private
	//


	//
	// Definitions

	namespace details
	{

		handler::handler(const signature_t &signature) :
			_signature(signature)
		{
		}


		handler::~handler()
		{
		}


		const signature_t &handler::signature() const
		{
			return _signature;
		}

	}

}

