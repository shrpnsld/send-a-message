# send-a-message

Library for inter-thread message passing.

- [Usage](#usage)
- [Example](#example)
- [Notes](#notes)



# Usage

Thread should be started using `sam::receivable_thread` function. It is similar to `std::thread` constructor.

`sam::receive` function runs message receiving loop. As handlers it accepts any [Callables](http://en.cppreference.com/w/cpp/concept/Callable) that return `void` or any of the following `sam::ctlcode_t` values:
* `sam::CONTINUE` - continue running message receiving loop (same as `void` function)
* `sam::STOP` - stop running message receiving loop

`sam::receive_for` accepts also a timeout parameter.

To send a message the member-function `sam::mailbox::send` should be called on an instance of a `sam::mailbox`.

The described interface is declared as follows:

```C++
template <typename Function_t, typename ...Arguments_t>
std::thread sam::receivable_thread(Function_t function, Arguments_t &&...arguments);
```

```C++
template <typename ...Callables_t>
void sam::receive(Callables_t &&...callables);
```

```C++
template <typename Rep, typename Period, typename ...Callables_t>
void receive_for(const std::chrono::duration<Rep, Period> &timeout, Callables_t &&...callables);
```

```C++
sam::mailbox::mailbox(const std::thread &thread);
```

```C++
template <typename ...Arguments_t>
sam::mailbox::send(Arguments_t &&...arguments);
```



# Example

Code:
```C++
#include "send_a_message.h"
```

```C++
// receiver-thread function
void receiver()
{
	// register handlers for each message type
	sam::receive(
		[](int i, double d)
		{
			printf("message: %i and %f", i, d);
		},

		[](const char *cstring)
		{
			printf("message: '%s'", cstring);
		}
	);
}
```

```C++
// start std::thread and asociate message queue with it
std::thread thread = sam::receivable_thread(receiver);

// get mailbox for the thread
sam::mailbox mailbox(thread);

// send messages
mailbox.send(42, M_PI);
mailbox.send("hellomoto!");
```

Output:
```
message: 42 and 3.141593
message: 'hellomoto!'
```



# Notes

* `sam::receivable_thread` function starts thread and creates a message queue for this thread. This message queue is destroyed after thread exit.

* `sam::mailbox` object is a wrapper for a reference to a message queue.

* Arguments of `sam::mailbox::send` member-function are forwarded to internal storage and they are copied/moved only once.

* Matching message to a handler is done by matching argument types ignoring reference specifiers. The matching is actually a search in a hash table.

