//
// Copyright (C) OpenSim Ltd.
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

#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/MACAddressTag_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/physicallayer/idealradio/IdealRadio.h"

namespace inet {

namespace physicallayer {

std::map<MACAddress, IdealRadio *> IdealRadio::idealRadios;

Define_Module(IdealRadio);

void IdealRadio::initialize(int stage)
{
    PhysicalLayerBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        bitrate = par("bitrate");
        preambleDuration = par("preambleDuration");
        headerBitLength = par("headerBitLength");
        propagationDelay = &par("propagationDelay");
        gate("radioIn")->setDeliverOnReceptionStart(true);
    }
    else if (stage == INITSTAGE_LINK_LAYER_2) {
        auto interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        auto interfaceEntry = interfaceTable->getInterfaceByInterfaceModule(this);
        auto address = interfaceEntry->getMacAddress();
        idealRadios[address] = this;
    }
}

void IdealRadio::handleMessageWhenUp(cMessage *message)
{
    if (message->getArrivalGate() == gate("radioIn"))
        receiveFromPeer(check_and_cast<cPacket *>(message));
    else
        PhysicalLayerBase::handleMessageWhenUp(message);
}

void IdealRadio::handleUpperPacket(cPacket *packet)
{
    auto destination = packet->getMandatoryTag<MACAddressReq>()->getDestinationAddress();
    if (destination.isBroadcast()) {
        for (auto it : idealRadios)
            if (it.second != this)
                sendToPeer(packet->dup(), it.second);
        delete packet;
    }
    else {
        auto peer = findPeer(destination);
        if (peer != nullptr)
            sendToPeer(packet, peer);
        else
            throw cRuntimeError("IdealRadio not found");
    }
}

void IdealRadio::handleLowerPacket(cPacket *packet)
{
    throw cRuntimeError("Received lower packet is unexpected");
}

IdealRadio *IdealRadio::findPeer(MACAddress address)
{
    auto it = idealRadios.find(address);
    if (it == idealRadios.end())
        return nullptr;
    else
        return it->second;
}

void IdealRadio::sendToPeer(cPacket *packet, IdealRadio *peer)
{
    simtime_t transmissionDuration = preambleDuration + (headerBitLength + packet->getBitLength()) / bitrate;
    packet->setDuration(transmissionDuration);
    sendDirect(packet, propagationDelay->doubleValue(), transmissionDuration, peer->gate("radioIn")->getPathStartGate());
}

void IdealRadio::receiveFromPeer(cPacket *packet)
{
    emit(packetSentToUpperSignal, packet);
    send(packet, gate("upperLayerOut"));
}

} // namespace physicallayer

} // namespace inet

