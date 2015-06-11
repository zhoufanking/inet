//
// Copyright (C) 2014 Irene Ruengeler
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/applications/tunapp/TunnelApp.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace inet {

Define_Module(TunnelApp);

TunnelApp::TunnelApp()
{
}

TunnelApp::~TunnelApp()
{
}

void TunnelApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        tunInterface = par("tunInterface");
        destAddress = par("destAddress");
        destPort = par("destPort");
        localPort = par("localPort");

        // TODO: use tun device socket?
        IInterfaceTable *interfaceTable = check_and_cast<IInterfaceTable *>(getParentModule()->getSubmodule("interfaceTable"));
        InterfaceEntry *interfaceEntry = interfaceTable->getInterfaceByName(tunInterface);
        cModule* interfaceModule = interfaceEntry->getInterfaceModule();
        gate("tunOut")->connectTo(interfaceModule->gate("appIn"));
        interfaceModule->gate("appOut")->connectTo(gate("tunIn"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        serverSocket.setOutputGate(gate("socketOut"));
        clientSocket.setOutputGate(gate("socketOut"));
        if (localPort != -1)
            serverSocket.bind(localPort);
        if (destPort != -1)
            clientSocket.connect(L3AddressResolver().resolve(destAddress), destPort);
    }
}

void TunnelApp::handleMessageWhenUp(cMessage *message)
{
    if (message->arrivedOn("socketIn")) {
        if (message->getKind() == UDP_I_DATA) {
            delete message->removeControlInfo();
            Ieee802Ctrl *controlInfo = new Ieee802Ctrl();
            controlInfo->setNetworkProtocol(ETHERTYPE_IPv4);
            message->setControlInfo(controlInfo);
            send(message, "tunOut");
        }
        else
            throw cRuntimeError("Unknown message");
    }
    else if (message->arrivedOn("tunIn")) {
        cPacket *packet = check_and_cast<cPacket *>(message);
        delete packet->removeControlInfo();
        clientSocket.send(packet);
    }
    else
        throw cRuntimeError("Unknown message");
}

} // namespace inet

