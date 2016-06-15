#ifndef __CYTL_ABSTRACTFACTORY__
#define __CYTL_ABSTRACTFACTORY__

#include "Utils.hpp"

namespace CYTL
{
    // ---------------------------------------
    // Generic Abstract Factory

    // AbstractFactoryUnit
    template<class T>
    struct AbstractFactoryUnit
    {
        virtual T* DoCreate(Type2Type<T>) = 0;
        virtual ~AbstractFactoryUnit() {}
    };

    // AbstractFactory
    template<class PList, template<class> class Unit = AbstractFactoryUnit>
    struct AbstractFactory: public GenScatterHierarchy<PList, Unit>
    {
        using ProductList = PList;
        template<class T>
        T* Create()
        { return static_cast<Unit<T>&>(*this).DoCreate( Type2Type<T>() ); }
    };

    // ConcreteFactoryUnit
    template<class Product, class Base>
    struct ConcreteFactoryUnit: public Base
    {
        using AbstractProduct = TypeCar<typename Base::ProductList>;
        using ProductList = TypeCdr<typename Base::ProductList>;

        Product* DoCreate(Type2Type<AbstractProduct>) override
        { return new Product; }
    };

    // ConcreteFactory
    template<class AbstractFact, class ProductList, template<class, class> class Unit = ConcreteFactoryUnit>
    struct ConcreteFactory: public GenLinearHierarchy<TypeReverse<ProductList>, Unit, AbstractFact>
    {};

} // namespace CYTL

#endif // __CYTL_ABSTRACTFACTORY__