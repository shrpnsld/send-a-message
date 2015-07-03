#include "message.h"


namespace sam
{

	//
	// Definitions

	super_fields::~super_fields()
	{
	}


	message::message(std::type_index signature, std::shared_ptr<super_fields> data)
		: _signature(signature), _data(data)
	{
	}


	const std::type_index &message::signature() const
	{
		return _signature;
	}


	void *message::data()
	{
		return _data->void_ptr();
	}

}

