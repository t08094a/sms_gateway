/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Jochen Fähnlein <jochen.faehnlein@ff-ipsheim.de>
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

#include "SmsTradeUrl.h"
#include "ConfigReader.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

#include <boost/log/trivial.hpp>

const string SmsTradeUrl::name = "SmsTradeUrl";

SmsTradeUrl::SmsTradeUrl() : io_service(), gateway("gateway.smstrade.de/bulk/"), key(), route("direct"), from("FF Alarm")
{
    InitializeFromConfig();
}

SmsTradeUrl::SmsTradeUrl (const SmsTradeUrl& other)
{

}

SmsTradeUrl::~SmsTradeUrl()
{
    io_service.stop();
}

SmsTradeUrl& SmsTradeUrl::operator= (const SmsTradeUrl& other)
{

}

const string SmsTradeUrl::GetGatewayName()
{
    return name;
}

string SmsTradeUrl::GetName() const
{
    return name;
}

void SmsTradeUrl::InitializeFromConfig()
{
    /*
==========
config.ini
==========
[SmsTrade]
Key = 9VUtGMBbe151aaf4E38BNnp
Route = direct
From = FF Alarm
*/
    
    key = ConfigReader::GetInstance().Get("SmsTrade.Key");
    route = ConfigReader::GetInstance().Get("SmsTrade.Route");
    from = ConfigReader::GetInstance().Get("SmsTrade.From");
}

void SmsTradeUrl::SendMessage (const string& receivers, const string& msg)
{
    try
    {
        string httpPost = gateway + CreateParameterString(msg, receivers);
        
        using boost::asio::ip::tcp;
        
        // get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(gateway, "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        // try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);
        
        // form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        ostream request_stream(&request);
        
        string data = CreateParameterString(msg, receivers);
        
        request_stream << "POST /bulk/ HTTP/1.1" << endl;
        request_stream << "Host:" << gateway << endl;
        request_stream << "Accept: */*" << endl;
        request_stream << "Connection: close" << endl;
        request_stream << "Content-Type: application/x-www-form-urlencoded" << endl;
        request_stream << "Content-Length: " << data.length() << endl << endl;
        request_stream << data;
        
        // send the request.
        boost::asio::write(socket, request);
        
        // read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");
        
        // check that response is OK.
        istream response_stream(&response);
        string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        string status_message;
        getline(response_stream, status_message);
        
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            //BOOST_LOG_TRIVIAL(error) << "Invalid response" << endl;
        }
        else if (status_code != 200)
        {
            //BOOST_LOG_TRIVIAL(info) << "Response returned with status code " << status_code << endl;
        }
        else
        {
            // read the response headers, which are terminated by a blank line.
            boost::asio::read_until(socket, response, "\r\n\r\n");

            // process the response headers.
            string header;
            while (getline(response_stream, header) && header != "\r")
            {
                //BOOST_LOG_TRIVIAL(info) << header << endl;
            }

            // write whatever content we already have to output.
            if (response.size() > 0)
            {
                //BOOST_LOG_TRIVIAL(info) << "Response: " << &response << endl;
            }

            // read until EOF, writing data to output as we go.
            boost::system::error_code error;
            while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
            {
                //BOOST_LOG_TRIVIAL(info) << &response;
            }

            if (error != boost::asio::error::eof)
            {
                throw boost::system::system_error(error);
            }
        }
    }
    catch (std::exception& e)
    {
        // BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what() << endl;
    }
}

void SmsTradeUrl::SendMessage (const string& to, const string& msg, const map< string, string >& options)
{
    //BOOST_LOG_TRIVIAL(info) << "Send Alarm SMS" << endl;
}

string SmsTradeUrl::CreateParameterString (const string& msg, const string& receivers)
{
    string encodedMsg = UrlEncode(msg);
    
    const char separator = '&';
    const char activate = '1';
    
    stringstream ss;
    
    ss << "?";
    ss << "key=" << key << separator;
    ss << "message=" << encodedMsg << separator;
    ss << "to=" << receivers << separator;
    ss << "from=" << from << separator;
    ss << "route=" << route << separator;
    //ss << "count=" << activate << separator;
    ss << "debug=" << activate << separator;
    ss << "dlr=" << activate << separator;
    ss << "messagetype=" << "flash" << separator;
    
    
    return ss.str();
}

string SmsTradeUrl::UrlEncode (const string& input)
{
    //RFC 3986 section 2.3 Unreserved Characters (January 2005)
    const string unreserved = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
    
    string escaped="";
    
    for(size_t i=0; i<input.length(); i++)
    {
        if (unreserved.find_first_of(input[i]) != string::npos)
        {
            escaped.push_back(input[i]);
        }
        else if(input[i] == ' ')
        {
            escaped.append("+");
        }
        else
        {
            escaped.append("%");
            char buf[3];
            sprintf(buf, "%.2X", input[i]);
            escaped.append(buf);
        }
    }

    return escaped;
}
