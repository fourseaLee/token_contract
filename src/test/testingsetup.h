#ifndef TESTINGSETUP_H
#define TESTINGSETUP_H

#include <boost/test/unit_test.hpp>


struct BasicTestingSetup
{
    BasicTestingSetup();
    ~BasicTestingSetup();
};

struct  TestingSetup
{
    TestingSetup();
    ~TestingSetup();
};

struct test_fixture_name
{
    test_fixture_name(){}
    ~test_fixture_name(){}
};

#endif
