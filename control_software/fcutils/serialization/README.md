# fcutils/serialization

A couple of class to serialize into a [std::string](https://en.cppreference.com/w/cpp/string/basic_string) and deserialize, from an array of bytes, an object, or an array of objects, of the group of integer types or PODs. 

Both classes use a serialization or deserialization policy, that can be specialized as a template type. 
A policy class must be a *DefaultConstrutible* functional type, having the *operator()* type: 

```cpp
template<typename ToBeSerialized> 
void operator()(std::string & t, const ToBeSerialized *s, std::size_t length=sizeof(ToBeSerialized))
```

Actually two different policies have been created: ***representation***  or ***string***.

The ***representation*** policy, stores the word representation, a byte at a time, into a [std::string](https://en.cppreference.com/w/cpp/string/basic_string). For example an integer with the value: 43981 (0xABCD), will be serialized into a string: "0xabcd", an array of char with the value: 'C','i','a','o', will be serialized into the string: "0x6369616f". The prefix "0x", can be omitted, or substituted with another prefix, optionally configurable when creating the serialization object. 

The ***string*** policy, stores the word value, into a [std::string](https://en.cppreference.com/w/cpp/string/basic_string). For example an integer with the value: 43981 (0xABCD), will be serialized into a string: "43981", an array of char with the value: 'C','i','a','o', will be serialized into the string: "Ciao"

Viceversa the deserialization works with a serialized array of char and deserialize it into an object according to the above mentioned policies. For example a deserialization object created with the policy: ***representation***, will deserialize an array of char: '0','x','a','b','c', into an integer with the value: 43981 (0xABCD). Instead a deserialization object, created with the policy: ***string***, will deserialize an array of char: '0','x','a','b','c', into a string: "0xabcd". In the case of ***representation*** policy, the deserialize will automatic detect the "0x" prefix and get rid of it. 

The two classes are defined into:
- **[deserializer.hpp](deserializer.hpp)**;
- **[serializer.hpp](serializer.hpp)**.

For convenience types with predefined policies have been defined: 

- **fcutils::serialization::serializer_repr**: the serializer type with the policy ***representation***;
- **fcutils::serialization::serializer_string**: the serializer type with the policy ***string***;
- **fcutils::serialization::deserializer_repr**: the deserializer type with the policy ***representation***;
- **fcutils::serialization::deserializer_string**: the deserializer type with the policy ***string***;

An example of usage:

```cpp
	namespace fcser=fcutils::serialization;
...
	struct test{
		// We need to be aligned to the largest type ....
		uint8_t id[2]{0xFAu,0xB0u};
		uint16_t i16[3];
	};
	uint16_t count{0u};
	test c;
	// Create serializers and deserializers
	fcser::serialize_repr b;
	fcser::deserialize_repr db,ddb;
	// Fill the test structure
	c.i16[0]=++count;
	c.i16[1]=count*2u;
	c.i16[2]=count*3u;
	// Clear the serializer
	b.clear();
	// Add prefix
	b+="0x";
	// Serialize the data
	b.serialize(&c);
	// Create empty objects
	test d{},dd{};
	// Deserialize a b.size() bytes
	auto resdb=db.deserialize(&d,b.c_str(),b.size());
	// Auto deduce the number of bytes based on the size of the type into wich we have to serialize. 
	auto resdd=ddb.deserialize(&dd,b.c_str());
...
	std::string val,deser;
	fcser::serialize_string a;
	fcser::deserialize_string da;
	val="Ciccio";
	// Clear the deserializer
	a.clear();
	// Reserve a given amount of bytes in the internal string object
	a.reserve(val.size());
	// Serialize the string
	a.serialize(val.c_str(),val.size());
	// Reserve the number of bytes to be written in the deserialized string
	deser.reserve(a.data().size());
	// Deserialize the a string into a new string
	da.deserialize(&deser,a.c_str(),a.data().size() );
...

```

