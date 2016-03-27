#include "message.h"


namespace sam { namespace detail
{

	const signature_t message::signature() const noexcept
	{
		return _signature;
	}


	void *message::data() noexcept
	{
		return reinterpret_cast<void *>(&_data);
	}

}
}

