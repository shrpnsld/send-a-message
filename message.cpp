#include "message.h"


namespace sam
{

	namespace details
	{

		//
		// Definitions

		message::message(signature_t signature) :
			_signature{signature}
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

