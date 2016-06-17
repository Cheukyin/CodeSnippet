#ifndef __CYTL_SINGLETON__
#define __CYTL_SINGLETON__

#include "Utils.hpp"
#include <stdexcept>
#include <cstdlib>

namespace CYTL
{
    // ------------------------
    // Regular Singleton
    class RegularSingleton: public NonCopyable
    {
    public:
        RegularSingleton* getInstance()
        {
            if(!pInstance_)
            { if(isDestroyed_) onDeadReference(); }
            else createInstance();

            return pInstance_;
        }

        ~RegularSingleton()
        {
            pInstance_ = nullptr;
            isDestroyed_ = false;
        }

    private:
        static RegularSingleton* pInstance_;
        static bool isDestroyed_;

        static void createInstance()
        {
            static RegularSingleton instance;
            pInstance_ = &instance;
        }

        static void onDeadReference()
        { throw std::runtime_error("Dead Reference Detected"); }
    };


    // ------------------------
    // Phoenix Singleton
    class PhoenixSingleton: public NonCopyable
    {
    public:
        PhoenixSingleton* getInstance()
        {
            if(!pInstance_)
            { if(isDestroyed_) onDeadReference(); }
            else createInstance();

            return pInstance_;
        }

        ~PhoenixSingleton()
        {
            pInstance_ = nullptr;
            isDestroyed_ = false;
        }

    private:
        static PhoenixSingleton* pInstance_;
        static bool isDestroyed_;

        static void createInstance()
        {
            static PhoenixSingleton instance;
            pInstance_ = &instance;
        }

        static void killPhoenixSingleton()
        { pInstance_->~PhoenixSingleton(); }

        static void onDeadReference()
        {
            createInstance();
            new (pInstance_) PhoenixSingleton;
            std::atexit(killPhoenixSingleton);
            isDestroyed_ = false;
        }
    };

}

#endif // __CYTL_SINGLETON__