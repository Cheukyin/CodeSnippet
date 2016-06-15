#include "AbstractFactory.hpp"
#include "UnitTest.hpp"
#include <typeinfo>
#include <string>

namespace
{
    struct Enemy { virtual std::string name() { return "Enemy"; }; virtual ~Enemy() {}; };

    struct Soldier: Enemy { std::string name() override { return "Soldier"; }; };
    struct SimpleSoldier: Soldier { std::string name() override { return "SimpleSoldier"; }; };
    struct HardSoldier: Soldier { std::string name() override { return "HardSoldier"; }; };

    struct Monster: Enemy { std::string name() override { return "Monster"; }; };
    struct SimpleMonster: Monster { std::string name() override { return "SimpleMonster"; }; };
    struct HardMonster: Monster { std::string name() override { return "HardMonster"; }; };

    struct SuperMonster: Enemy { std::string name() override { return "SuperMonster"; }; };
    struct SimpleSuperMonster: SuperMonster { std::string name() override { return "SimpleSuperMonster"; }; };
    struct HardSuperMonster: SuperMonster { std::string name() override { return "HardSuperMonster"; }; };

    using EnemyList = CYTL::TypeList<Soldier, Monster, SuperMonster>;
    using SimpleEnemyList = CYTL::TypeList<SimpleSoldier, SimpleMonster, SimpleSuperMonster>;
    using HardEnemyList = CYTL::TypeList<HardSoldier, HardMonster, HardSuperMonster>;
}

TEST_CASE(GenricAbstractFactory)
{
    using AbstractEnemyFactory = CYTL::AbstractFactory<EnemyList>;
    using SimpleEnemyFactory = CYTL::ConcreteFactory<AbstractEnemyFactory, SimpleEnemyList>;
    using HardEnemyFactory = CYTL::ConcreteFactory<AbstractEnemyFactory, HardEnemyList>;


    // -----------------------------------
    AbstractEnemyFactory *enemyFactory = new SimpleEnemyFactory;

    Enemy *e = enemyFactory->Create<Soldier>();
    EXPECT_EQ(e->name(), "SimpleSoldier");
    delete e;

    e = enemyFactory->Create<Monster>();
    EXPECT_EQ(e->name(), "SimpleMonster");
    delete e;

    e = enemyFactory->Create<SuperMonster>();
    EXPECT_EQ(e->name(), "SimpleSuperMonster");
    delete e;

    delete enemyFactory;


    // -----------------------------------
    enemyFactory = new HardEnemyFactory;

    e = enemyFactory->Create<Soldier>();
    EXPECT_EQ(e->name(), "HardSoldier");
    delete e;

    e = enemyFactory->Create<Monster>();
    EXPECT_EQ(e->name(), "HardMonster");
    delete e;

    e = enemyFactory->Create<SuperMonster>();
    EXPECT_EQ(e->name(), "HardSuperMonster");
    delete e;

    delete enemyFactory;
}