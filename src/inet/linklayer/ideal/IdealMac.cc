//
// Copyright (C) OpenSim Ltd
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

#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MACAddressTag_m.h"
#include "inet/linklayer/ideal/IdealMac.h"

namespace inet {

std::map<MACAddress, IdealMac *> IdealMac::idealMacs;

Define_Module(IdealMac);

void IdealMac::initialize(int stage)
{
    MACProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        bitrate = par("bitrate");
        propagationDelay = &par("propagationDelay");
        overheadBitLength = &par("overheadBitLength");
        overheadDuration = &par("overheadDuration");
        initializeMacAddress();
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        registerInterface();
    }
}

void IdealMac::initializeMacAddress()
{
    const char *addressString = par("address");
    if (!strcmp(addressString, "auto"))
        address = MACAddress::generateAutoAddress();
    else
        address.setAddress(addressString);
    idealMacs[address] = this;
}

InterfaceEntry *IdealMac::createInterfaceEntry()
{
    auto interfaceEntry = new InterfaceEntry(this);
    interfaceEntry->setDatarate(bitrate);
    interfaceEntry->setMACAddress(address);
    interfaceEntry->setInterfaceToken(address.formInterfaceIdentifier());
    interfaceEntry->setMulticast(false);
    interfaceEntry->setBroadcast(true);
    return interfaceEntry;
}

void IdealMac::handleMessageWhenUp(cMessage *message)
{
    if (message->getArrivalGate() == gate("peerIn"))
        receiveFromPeer(check_and_cast<cPacket *>(message));
    else
        MACProtocolBase::handleMessageWhenUp(message);
}

void IdealMac::handleUpperPacket(cPacket *packet)
{
    auto destination = packet->getMandatoryTag<MACAddressReq>()->getDestinationAddress();
    if (destination.isBroadcast()) {
        for (auto it : idealMacs)
            if (it.second != this)
                sendToPeer(packet->dup(), it.second);
        delete packet;
    }
    else {
        auto peer = findPeer(destination);
        if (peer != nullptr)
            sendToPeer(packet, peer);
        else
            throw cRuntimeError("IdealMac not found");
    }
}

void IdealMac::handleLowerPacket(cPacket *packet)
{
    throw cRuntimeError("Received lower packet is unexpected");
}

IdealMac *IdealMac::findPeer(MACAddress address)
{
    auto it = idealMacs.find(address);
    if (it == idealMacs.end())
        return nullptr;
    else
        return it->second;
}

void IdealMac::sendToPeer(cPacket *packet, IdealMac *peer)
{
    simtime_t transmissionDuration = (packet->getBitLength() + overheadBitLength->doubleValue()) / bitrate + overheadDuration->doubleValue();
    packet->setDuration(transmissionDuration);
    sendDirect(packet, propagationDelay->doubleValue(), transmissionDuration, peer->gate("peerIn"));
}

void IdealMac::receiveFromPeer(cPacket *packet)
{
    auto protocol = packet->getMandatoryTag<PacketProtocolTag>()->getProtocol();
    packet->clearTags();
    packet->ensureTag<DispatchProtocolReq>()->setProtocol(protocol);
    packet->ensureTag<InterfaceInd>()->setInterfaceId(interfaceEntry->getInterfaceId());
    sendUp(packet);
}

} // namespace inet

