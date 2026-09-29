///
///
/// @brief A function to wait for all futures in a container of futures. Thanks to B.S. for the inspiration (See §42.4.4 of The C++ Programming language - 4th ed.)
///
/// @file wait_for_all.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 28 giu 2022 
///

#ifndef WAIT_FOR_ALL_HPP_
#define WAIT_FOR_ALL_HPP_
#include <future>

namespace fcutils{
namespace concurrency{

template<typename T, template<typename> class Container>
Container<T> wait_for_all(Container<std::future<T>>& vf) {
		Container<T> res;
		for (auto& fu : vf)
			res.push_back(fu.get());
		return res;
}

} // namespace concurrency
} // namespace fcutils

#endif // WAIT_FOR_ALL_HPP_ 
