#include <cstddef>

namespace CYTL
{

    // return size of an array as a constant-time constant
    template<typename T, std::size_t N>
    constexpr std::size_t arraySize(T (&)[N]) noexcept
    { return N; }


    // Type Displayer
    // usage: TypeDisplayer<decltype(t)> tType;
    //        then see the compling error info
    template<typename T> class TypeDisplayer;

} // namespace CYTL