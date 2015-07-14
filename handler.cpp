#include "handler.h"


namespace sam
{

	namespace details
	{

		//
		// Definitions

		handler::handler(const signature_t &signature) :
			_signature{signature}
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

