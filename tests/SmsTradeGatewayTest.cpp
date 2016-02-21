#define BOOST_TEST_DYN_LINK


#include <boost/test/unit_test.hpp>
#include "SmsTradeGateway.h"

using namespace boost::unit_test;
using namespace std;


BOOST_AUTO_TEST_CASE( GetNameReturnsSmstrade )
{
    string expected = "SmsTrade";
    
    SmsTradeGateway gateway;
    string name = gateway.GetName();
    
    BOOST_CHECK(expected == name);
}

BOOST_AUTO_TEST_CASE( GetGatewayNameReturnsSmstrade )
{
    string expected = "SmsTrade";
    
    BOOST_CHECK(expected == SmsTradeGateway::GetGatewayName());
}

BOOST_AUTO_TEST_CASE( SendMessage )
{
    // TODO: Debugmodus aktuell nicht möglich -> es wird immer eine SMS gesandt
    /*
    string to = "01749441938"; //"Alle";
    string msg = "Hello World";
    
    map<string, string> options;
    options["debug"] = "1";           // Aktiviert den Debugmodus. SMS werden nicht versendet und nicht berechnet
    options["message_id"] = "1";      // Aktiviert die Rückgabe der Message ID
    options["count"] = "1";           // Aktiviert die Rückgabe der SMS Anzahl
    options["dlr"] = "1";             // Aktiviert den Empfang eines Versandberichtes für diese SMS
    options["messagetype"] = "flash"; // Typ der Nachricht: flash, unicode, binary, voice
    
    SmsTradeGateway gateway;
    
    gateway.SendMessage(to, msg, options);
    */
}