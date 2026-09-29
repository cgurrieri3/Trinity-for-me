///
///
/// @brief A function to wait for the first future available out of a collection. Thanks to B.S. for the inspiration (See §42.4.4 of The C++ Programming language - 4th ed.).
///
/// @file wait_for_any.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 28 giu 2022 
///

#ifndef WAIT_FOR_ANY_HPP_
#define WAIT_FOR_ANY_HPP_
#include <future>
#include <chrono>
#include <thread>

namespace fcutils{
namespace concurrency{

template<typename T, template <typename> class Container>
int wait_for_any(Container<std::future<T>>& vf, std::chrono::steady_clock::duration d){
	// return index of ready future
	// if no future is ready, wait for d before trying again
	// I decided to consider a deferred task (§42.4.6) an error for my uses.
	while(true) {
		for (int i=0; i!=vf.size(); ++i) {
			if (!vf[i].valid()) continue;
			switch (vf[i].wait_for(std::chrono::seconds{0})) {
			case std::future_status::ready:
				return i;
			case std::future_status::timeout:
				break;
			case std::future_status::deferred:
				throw std::runtime_error("wait_for_any(): deferred future");
			} // switch
		} // for
		std::this_thread::sleep_for(d);
	}
}

} // namespace concurrency
} // namespace fcutils



#endif // WAIT_FOR_ANY_HPP_ 
