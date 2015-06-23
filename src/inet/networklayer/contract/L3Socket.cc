//
// Copyright (C) 2013 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/networklayer/contract/L3Socket.h"
#include "inet/networklayer/contract/L3SocketCommand_m.h"

namespace inet {

L3Socket::L3Socket(int controlInfoProtocolId, cGate *gateToIP) :
    controlInfoProtocolId(controlInfoProtocolId),
    socketId(getEnvir()->getUniqueNumber()),
    gateToIP(gateToIP)
{
}

void L3Socket::setControlInfoProtocolId(int _controlInfoProtocolId)
{
    ASSERT(!bound);
    controlInfoProtocolId = _controlInfoProtocolId;
}

void L3Socket::sendToIP(cMessage *message)
{
    if (!gateToIP)
        throw cRuntimeError("L3Socket: setOutputGate() must be invoked before the socket can be used");
    check_and_cast<cSimpleModule *>(gateToIP->getOwnerModule())->send(message, gateToIP);
}

void L3Socket::bind(int protocolId)
{
    ASSERT(!bound);
    ASSERT(controlInfoProtocolId != -1);
    L3SocketBindCommand *command = new L3SocketBindCommand();
    command->setControlInfoProtocolId(controlInfoProtocolId);
    command->setSocketId(socketId);
    command->setProtoclId(protocolId);
    cMessage *bind = new cMessage("bind");
    bind->setControlInfo(command);
    sendToIP(bind);
    bound = true;
}

void L3Socket::send(cPacket *msg)
{
    sendToIP(msg);
}

void L3Socket::close()
{
    ASSERT(bound);
    ASSERT(controlInfoProtocolId != -1);
    L3SocketCloseCommand *command = new L3SocketCloseCommand();
    command->setControlInfoProtocolId(controlInfoProtocolId);
    command->setSocketId(socketId);
    cMessage *close = new cMessage("close");
    close->setControlInfo(command);
    sendToIP(close);
}

} // namespace inet

