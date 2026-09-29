# fcutils/utility

A collection of utilities:

- **[apply_modifier.hpp](apply_modifier.hpp)**:
An utility help function similar to the [std::apply](https://en.cppreference.com/w/cpp/utility/apply), for [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) but invoking a function that modifies the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) element. So, it returns void and leverage fold expressions.
- **[byte_iostream.hpp](byte_iostream.hpp)**:
Define an input and oputput stream operators for [std::byte](https://en.cppreference.com/w/cpp/types/byte).
- **[extractor.hpp](extractor.hpp)**:
To extract a value from an integer with a given bit size, giving an offset and a mask.
- **[finally.hpp](finally.hpp)**:
As an alternative to exception throw and [RAII](https://en.cppreference.com/w/cpp/language/raii) (Resource Acquisition Is Initialization): the [finally pattern](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Re-finally).To create an object that will execute an action at the end of its life.
- **[inserter.hpp](inserter.hpp)**:
To insert a value inside an integer given an offset and a mask.
- **[overload.hpp](overload.hpp)**:
The overload pattern. Used to visit a [std::variant](https://en.cppreference.com/w/cpp/utility/variant).  
- **[print_vals.hpp](print_vals.hpp)**:
To print a list of values to an output stream. Values are separated by a constant delimiter.
- **[stream_tuple.hpp](stream_tuple.hpp)**:
To stream an entuple to an output stream (std::ostream).
- **[utility.hpp](utility.hpp)**:
A convenience header to include all header files.
