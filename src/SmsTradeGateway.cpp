/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  Jochen Fähnlein <jochen.faehnlein@ff-ipsheim.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "SmsTradeGateway.h"
#include "SmstradeBinding.nsmap" // get the gSOAP-generated namespace bindings
#include "soapSmstradeBindingProxy.h"

const string SmsTradeGateway::name = "smstrade";

SmsTradeGateway::SmsTradeGateway()
{
    // nop
}

SmsTradeGateway::~SmsTradeGateway()
{
    // nop
}

const string SmsTradeGateway::GetGatewayName()
{
    return name;
}

string SmsTradeGateway::GetName() const
{
    return name;
}

void SmsTradeGateway::SendMessage(const string& to, const string& msg) // to = Empfänger der Nachricht (Integer, bis zu 16 Zeichen)
{
    string key = "9VUtGMBbe151aaf4E38BNnp";         // Persönlicher Identifikationscode (String, bis zu 35 Zeichen)
    //msg = "Hallo Test";                             // Nachrichtentext (String, bis zu 1530 Zeichen)
    string route = "direct";                        // Auswahl der SMS-Route (basic|economy|gold|direct)
    string from = "FF ALARM";                       // Absenderkennung der SMS (String, bis zu 11 Zeichen; Integer, bis zu 16 Zeichen)
    
    SmstradeBindingProxy server;
    server.soap->connect_timeout = 10; // connect within 10 s
    server.soap->send_timeout = 5; // send timeout is 5s
    server.soap->recv_timeout = 5; // receive timeout is 5s
    
    try
    {
        string setOptionalParamResponse;
        
        // Aktiviert den Debugmodus. SMS werden nicht versendet und nicht berechnet
        int result = server.setOptionalParam("debug", "1", setOptionalParamResponse);
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
        
        // Aktiviert die Rückgabe der Message ID
        result = server.setOptionalParam("message_id", "1", setOptionalParamResponse);
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
        
        // Aktiviert die Rückgabe der SMS Anzahl
        result = server.setOptionalParam("count", "1", setOptionalParamResponse);
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
        
        // Aktiviert den Empfang eines Versandberichtes für diese SMS
        result = server.setOptionalParam("dlr", "1", setOptionalParamResponse);
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
        
        // Zeichenkodierung der Parameter "message" und "from"
        result = server.setOptionalParam("charset", "UTF-8", setOptionalParamResponse);
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
        
        // Typ der Nachricht: flash, unicode, binary, voice
        result = server.setOptionalParam("messagetype", "flash", setOptionalParamResponse);
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
    }
    catch(...)
    {
        
    }
    
    try
    {
        struct ns1__sendSMSResponse returnData;
        
        // returns error code or SOAP_OK
        int result = server.sendSMS(key, to, msg, route, from, returnData); // $returnval Array mit Daten: 0 => Returncode, 1 => MessageID, 2 => entstandene Kosten, 3 => Anzahl der SMS, 4 => Zeitpunkt des Versandes
        if(result != SOAP_OK)
        {
            server.soap_stream_fault(std::cerr);
        }
        
        /*
        string returncode = returnData.sendSMSResponse[0];
        string messageId = returnData.sendSMSResponse[1];
        string cost = returnData.sendSMSResponse[2];
        */
    }
    catch(...)
    {
        
    }
    
    server.destroy();
}
