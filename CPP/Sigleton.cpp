#include "Singleton.hpp"

namespace CYTL
{
    RegularSingleton* RegularSingleton::pInstance_ = nullptr;
    bool RegularSingleton::isDestroyed_ = false;

    PhoenixSingleton* PhoenixSingleton::pInstance_ = nullptr;
    bool PhoenixSingleton::isDestroyed_ = false;
}