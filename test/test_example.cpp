#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(example_test)
{
    int x = 2;
    int y = 3;
    BOOST_TEST((x + y) == 5);
}
