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

#include "../include/GatewayManager.h"
#include "../include/GatewayFactory.h"

GatewayManager::GatewayManager()
{
    // nop
}

GatewayManager::~GatewayManager()
{
    // nop
}

const set<string> GatewayManager::GetPossibleGatewayNames() const
{
    return GatewayFactory::GetInstance().GetAvailableGatewayNames();
}
    
void GatewayManager::SendMessage(std::string gateway, std::string distributionList, std::string msg)
{
    msg = "Hallo Test";                             // Nachrichtentext (String, bis zu 1530 Zeichen)
    
    GatewayFactory::GetInstance().GetGateway("gateway").get()->SendMessage(distributionList, msg);
}