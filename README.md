# send-a-message

Library for inter-thread message passing.

- [Usage](#usage)
- [Example](#example)
- [Notes](#notes)



# Usage

Thread should be started using `sam::receivable_thread` function. It is similar to `std::thread` constructor.

`sam::receive` function runs message receiving loop. It accepts function pointers and `std::function` as handlers. They can be `void` or can return any of the following `sam::ctlcode_t` values:
* `sam::CONTINUE` - continue running message receiving loop (same as `void` function)
* `sam::STOP` - stop running message receiving loop

To send a message the member-function `sam::mailbox::send` should be called on an instance of a `sam::mailbox`.

The described interface is declared as follows:

```C++
template <typename Function_t, typename ...Arguments_t>
std::thread sam::receivable_thread(Function_t function, Arguments_t &&...arguments);
```

```C++
template <Functions_t ...functions>
void sam::receive(Functions_t &&...functions);
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
// handlers (with code that prints function signature)
void handler_int_float(int i, float f);
void handler_cstring(const char *);

// receiver-thread function
void receiver()
{
	// register handlers for each message type
	sam::receive(
		handler_int_float,
		handler_cstring,
	);
}
```

```C++
// start std::thread and asociate message queue with it
std::thread thread(sam::receivable_thread(receiver));

// get mailbox for the thread
sam::mailbox mailbox(thread);

// send messages
mailbox.send(42, 3.14159f);
mailbox.send("hellomoto!");
```

Output:
```
void handle_int_float(int, float)
void handle_cstring(const char *)
```



# Notes

* `sam::receivable_thread` function starts thread and creates a message queue for this thread. This message queue is destroyed after thread exit.

* `sam::mailbox` object is a wrapper for a reference to a message queue.

* Arguments of `sam::mailbox::send` member-function are forwarded to internal storage and they are copied/moved only once.

* Matching message to a handler is done by matching argument types ignoring reference specifiers. The matching is actually a search in a hash table.

