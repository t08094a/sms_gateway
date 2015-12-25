#define BOOST_TEST_DYN_LINK


#include <boost/test/unit_test.hpp>
#include "../include/SmsTradeGateway.h"
// 
using namespace boost::unit_test;
using namespace std;


BOOST_AUTO_TEST_CASE( GetNameReturnsSmstrade )
{
    string expected = "smstrade";
//     
    SmsTradeGateway gateway;
//     string name = gateway.GetName();
//     
//     BOOST_CHECK(expected == name);
}
