# send-a-message

C++11 inter-thread message passing library.


### Features

* Uses only C++11 and standard library
* Crossplatform. Tested with:
	* Apple LLVM version 7.3.0 (clang-703.0.29)
	* GCC 4.9.2
	* MinGW 4.9.3
	* Microsoft Visual Studio 2015
* Free for all ([MIT license](LICENSE.txt)) 



### Example

Code:

```c++
#include "send-a-message.hpp"
```

```c++
// receiver-thread function
void receiver()
{
	// run message receving-handling loop
	sam::receive(
		// handler for "int" and "double" values sent to thread
		[](int i, double d)
		{
			printf("message: %i and %f\n", i, d);
		},

		// handler for "const char *" value sent to thread
		[](const char *c_string)
		{
			printf("message: '%s'\n", c_string);
		}
	);
}
```

```c++
// start "std::thread" and associate message queue with it
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


# Usage

**First**, start your thread using `sam::receivable_thread` function:

```c++
template <typename Callable_t, typename ...Arguments_t>
std::thread sam::receivable_thread(Callable_t &&callable, Arguments_t &&...arguments);
```

This function creates new thread, associates new message queue with it and calls `callable` with forwarded `arguments...`. After `callable` returns the message queue is destroyed automatically.

**Second**, call `sam::receive` or `sam::receive_for` function anywhere in your thread to run message receiving-handling loop:

```c++
template <typename ...Callables_t>
void sam::receive(Callables_t &&...callables);
```

```c++
template <typename Rep_t, typename Period_t, typename ...Callables_t>
void sam::receive_for(const std::chrono::duration<Rep_t, Period_t> &timeout, Callables_t &&...callables);
```

These functions accept any number of [Callables](http://en.cppreference.com/w/cpp/concept/Callable). These Callables should return `void` or `sam::ctlcode_t` values which affect loop execution:

* `sam::CONTINUE` – continue running message receiving loop (`void` functions behave the same)
* `sam::STOP` – stop running message receiving loop

**Third**, to send a message get thread's mailbox and call member function `sam::mailbox::send` on it.

```c++
sam::mailbox::mailbox(const std::thread &thread);
```

```c++
template <typename ...Arguments_t>
sam::mailbox::send(Arguments_t &&...arguments);
```

Parameters of `sam::mailbox::send` are forwarded to internal storage, so they are copied/moved only once.



### Notes

There is a default handler for `sam::ctlcode_t` in `sam::receive` and `sam::receive_for` that simply returns it's argument. So you can stop message receiving-handling loop from the outside by sending control code `sam::STOP`. This handler can be overriden by adding callable that accepts `sam::ctlcode_t` as it's argument.

There is a default handler for `sam::timeout_error` that is called if `sam::receive_for` function hasn't received any messages during `timeout` period. This handler calls `std::this_thread::yield()` function. This handler can be overriden by adding callable that accepts `sam::timeout_error` as it's argument.

Matching message to a handler is done by matching argument types ignoring reference specifiers. These types are packed to `std::type_index` value and the matching itself is a hash table search.
