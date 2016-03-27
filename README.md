# send-a-message

Library for inter-thread message passing.

- [Usage](#usage)
- [Example](#example)
- [Notes](#notes)



# Usage

To enable thread for message receving, it should be started using `sam::receivable_thread` function. It's interface is similar to `std::thread` constructor.

`sam::receive` function runs message receiving loop. As handlers it accepts any [Callables](http://en.cppreference.com/w/cpp/concept/Callable) that return `void` or any of the following `sam::ctlcode_t` values:
* `sam::CONTINUE` - continue running message receiving loop (same as `void` function)
* `sam::STOP` - stop running message receiving loop

`sam::receive_for` function also accepts a timeout parameter.

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
template <typename Rep_t, typename Period_t, typename ...Callables_t>
void sam::receive_for(const std::chrono::duration<Rep_t, Period_t> &timeout, Callables_t &&...callables);
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
			printf("message: %i and %f\n", i, d);
		},

		[](const char *cstring)
		{
			printf("message: '%s'\n", cstring);
		}
	);
}
```

```C++
// start std::thread and asociate message queue with it
std::thread thread {sam::receivable_thread(receiver)};

// get mailbox for the thread
sam::mailbox mailbox {thread};

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

* The message queue for receivable thread is created before the user function call and destroyed after return.

* `sam::mailbox` object is a wrapper for a reference to a message queue, so it is [CopyConstructible](http://en.cppreference.com/w/cpp/concept/CopyConstructible).

* Arguments of `sam::mailbox::send` member-function are forwarded to internal storage and they are copied/moved only once.

* Matching message to a handler is done by matching argument types ignoring reference specifiers. The matching is actually a search in a hash table that uses `std::type_index` for a key type.

* There is a default handler for `sam::ctlcode_t` in `sam::receive`/`sam::receive_for` that simply returns it's argument, thus by sending control code `sam::STOP` to a thread the receive loop can be stopped from the outside. This handler can be overriden by adding callable that accepts `sam::ctlcode_t`.

* There is a default handler for `sam::timeout_error` that is called if `sam::receive_for` function hasn't received any messages during `timeout` period. This handler calls `std::this_thread::yield()` function. This handler can be overriden by adding callable that accepts `sam::timeout_error`.

