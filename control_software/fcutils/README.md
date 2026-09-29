# fcutils

A collection of general utilities useful to build SPB2 control software and more.

- **[algorithm](algorithm)**: 
Same as STL Algorithm. At the moment there is a clamp algorithm … if you do not use C++17 std::clamp.
- **[checker](checker)**:
A collection of packet checkers and return policies to be used by the serial port, Can bus and TCP/UDP  client/server. In this way users can define protocols to be used to check packet transmission in a communication, not having to change the class that implements the packet exchange. 
- **[chrono](chrono)**:
Utilities related to the C++11 chrono library. A timer class is present at the moment. 
- **[command](command)**:
Utilities to implement a process able to store commands, sequences of commands, sequencers to execute commands or sequences with a given frequency, consumers to dump data to file or to UDP and TCP addresses, spawn operating system commands, and log the standard output to files or UDP and TCP addresses.  
- **[concurrency](concurrency)**:
Utilities related to the C++11 concurrency. Here are present:
	- *guarded_thread*. A thread joining on deletion;
	- *lockable_container*. A class to add a lock on any container type;
	- *sync_queue*. A class to implement a generic synchronized queue, useful in communication between threads;
	- *wait_for_all* and *wait_for_any*. Functions to wait for all or for any future.
- **[consumer](consumer)**:
A series of consumers and a manager to handle them. Consumers abilities range from simply print data or dump them to a text or binary files, or a network address and port along with collecting a bit of statistic on the consumer collections. The manager can be configured to add an header, a trailer or both, optionaly including a timestamp.
- **[iostream](iostream)**:
A collection of sink and standard output duplicators writing it to files, remote ip addresses and port, or to a syslog server.
- **[memory](memory)**:
Utilities related fo the C++17 polymorpich memory resources (PMR) and other memory mangers or pools.
- **[message](message)**:
Classes implementing headers or trailers with different payloads like CRC or timestamps. 
- **[monitor](monitor)**:  
Utilities to implement an ASCII monitor printing values in various format. The monitor can be completely configured choosing the variables to print, the alarm thresholds and colors. 
- **[networking](networking)**:
A collection of utilities for the BOOST ASIO library. For example asynchronous TCP and UDP servers and clients. Clients can be specialized using a packet checker and consumers. 
- **[property_tree](property_tree)**:
To navigate, print or fine a value in a BOOST property tree.
- **[random](random)**:
Utilities related to the C++11 random generations. A flat and Gaussian generator classes. 
- **[serialization](serialization)**:
Classes to serialize and deserialize plain old data (POD) objects or collection of values.  
- **[serialport](serialport)**:
Utilities related to the BOOST ASIO serial port management. A talk_to_serial port class implements asynchronous reading and synchronous writing to a serial port. 
The class needs two specific types:
	- *Checker*. A functor type implementing the data packet check. It must implement: std::size_t operator()(Data *, std::size), that return the number of bytes to read in the next read operation.
	- *Consumer*. A functor type implementing the consumer of the data. It must implement: void operator()(Data *, std::size), implementing the data usage. 
A little bit of features and checks are needed to be added, but, at this stage, it implement a basic protocol independent class representing a serial communication. A protocol using a data packet terminated with an End Of Line has been added in the test directory. 
- **[string](string)**:
A collection of utilities needed. A string with added timestamp.
- **[system](system)**: 
Operating system related facilities. The posix_queue class is saved here, to implement interprocess communication using posix queue.
- **[terminal](terminal)**:
A collection of utilities to use terminal colors, management and cursor position using ANSI escape sequences.  
- **[type_traits](type_traits)**:
Type traits related utilities not implemented in STL. For example: type_list, is_bool, is_stream_writable, is_stream_readable.
- **[units](units)**:
A class implementing MKS group of units and quantities. 
- **[utility](utility)**:
Am heterogeneous collection of functions and classes. Among them:
	- *extractor and inserter*. Two classes to extract or insert a given number of bits from or into an integer object, given an offset and a mask.
	- *integer*. Type class to obtain integer types given a bit length.
	- *finally*. A class to implement the finally pattern.
	- *overload*. A class to store overload methods for a std::variant visitor.
	- *print_vals*. To print a generalized number of values of different types, comma separated.
	- *operator<< for std::tuple*. To pretty-print a std::tuple to a std::ostream. 
	- *apply_modifier*. To modify contents of a std::tuple applying a function or a functor to them.  
- **[test](test)**:
A collection of test programs and examples of usage of the utilities.  