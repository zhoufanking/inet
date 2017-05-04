//
// Copyright (C) 2006 Andras Varga
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

#include <string.h>

#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/EtherTypeTag_m.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/linklayer/common/MACAddressTag_m.h"
#include "inet/linklayer/common/UserPriorityTag_m.h"
#include "inet/linklayer/ieee802/Ieee802LlcHeader_m.h"
#include "inet/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h"

#ifdef WITH_ETHERNET
#include "inet/linklayer/ethernet/EtherEncap.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#endif // ifdef WITH_ETHERNET


namespace inet {

namespace ieee80211 {

void Ieee80211MgmtAPBase::initialize(int stage)
{
    Ieee80211MgmtBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        mib->sta.isAp = true;
        mib->sta.isBssMember = true;
        const char *encDec = par("encapDecap").stringValue();
        if (!strcmp(encDec, "true"))
            encapDecap = ENCAP_DECAP_TRUE;
        else if (!strcmp(encDec, "false"))
            encapDecap = ENCAP_DECAP_FALSE;
        else if (!strcmp(encDec, "eth"))
            encapDecap = ENCAP_DECAP_ETH;
        else
            throw cRuntimeError("Unknown encapDecap parameter value: '%s'! Must be 'true','false' or 'eth'.", encDec);
    }
}

void Ieee80211MgmtAPBase::convertToEtherFrame(Packet *packet)
{
#ifdef WITH_ETHERNET
    packet->removePoppedHeaders();
    const auto& ieee80211MacHeader = packet->removeHeader<Ieee80211DataFrame>();
    const auto& ieee802SnapHeader = packet->removeHeader<Ieee802SnapHeader>();

    // create a matching ethernet frame
    const auto& ethframe = std::make_shared<EthernetIIFrame>();    //TODO option to use EtherFrameWithSNAP instead
    ethframe->setDest(ieee80211MacHeader->getAddress3());
    ethframe->setSrc(ieee80211MacHeader->getTransmitterAddress());
    ethframe->setEtherType(ieee802SnapHeader->getProtocolId());
    ethframe->setChunkLength(byte(ETHER_MAC_FRAME_BYTES - 4)); // subtract FCS

    // encapsulate the payload in there
    ethframe->markImmutable();
    packet->pushHeader(ethframe);
    EtherEncap::addPaddingAndFcs(packet);

    packet->ensureTag<DispatchProtocolReq>()->setProtocol(&Protocol::ethernet);
    packet->ensureTag<PacketProtocolTag>()->setProtocol(&Protocol::ethernet);
#else // ifdef WITH_ETHERNET
    throw cRuntimeError("INET compiled without ETHERNET feature!");
#endif // ifdef WITH_ETHERNET
}

void Ieee80211MgmtAPBase::convertFromEtherFrame(Packet *packet)
{
#ifdef WITH_ETHERNET
    auto ethframe = EtherEncap::decapsulate(packet);       // do not use const auto& : removePoppedChunks() delete it from packet

    const auto& ieee802SnapHeader = std::make_shared<Ieee802SnapHeader>();
    ieee802SnapHeader->setOui(0);
    // copy EtherType from original ieee80211MacHeader
    if (const auto& eth2frame = std::dynamic_pointer_cast<EthernetIIFrame>(ethframe))
        ieee802SnapHeader->setProtocolId(eth2frame->getEtherType());
    else if (const auto& snapframe = std::dynamic_pointer_cast<EtherFrameWithSNAP>(ethframe))
        ieee802SnapHeader->setProtocolId(snapframe->getLocalcode());
    else
        throw cRuntimeError("Unaccepted EtherFrame type: %s, contains no EtherType", ethframe->getClassName());

    const auto& ieee80211MacHeader = std::make_shared<Ieee80211DataFrame>();
    ieee80211MacHeader->setFromDS(true);
    // copy addresses from ethernet frame (transmitter addr will be set to our addr by MAC)
    ieee80211MacHeader->setReceiverAddress(ethframe->getDest());
    ieee80211MacHeader->setAddress3(ethframe->getSrc());

    // encapsulate payload
    packet->removePoppedChunks();
    packet->insertHeader(ieee802SnapHeader);
    packet->insertHeader(ieee80211MacHeader);
#else // ifdef WITH_ETHERNET
    throw cRuntimeError("INET compiled without ETHERNET feature!");
#endif // ifdef WITH_ETHERNET
}

void Ieee80211MgmtAPBase::encapsulate(Packet *packet)
{
    switch (encapDecap) {
        case ENCAP_DECAP_ETH:
#ifdef WITH_ETHERNET
            convertFromEtherFrame(check_and_cast<Packet *>(packet));
#else // ifdef WITH_ETHERNET
            throw cRuntimeError("INET compiled without ETHERNET feature, but the 'encapDecap' parameter is set to 'eth'!");
#endif // ifdef WITH_ETHERNET
            break;

        case ENCAP_DECAP_TRUE: {
            const auto& ieee802SnapHeader = std::make_shared<Ieee802SnapHeader>();
            ieee802SnapHeader->setOui(0);
            ieee802SnapHeader->setProtocolId(packet->getMandatoryTag<EtherTypeReq>()->getEtherType());
            packet->insertHeader(ieee802SnapHeader);

            const auto& ieee80211MacHeader = std::make_shared<Ieee80211DataFrame>();
            ieee80211MacHeader->setFromDS(true);

            // copy addresses from ethernet ieee80211MacHeader (transmitter addr will be set to our addr by MAC)
            ieee80211MacHeader->setAddress3(packet->getMandatoryTag<MacAddressReq>()->getSrcAddress());
            ieee80211MacHeader->setReceiverAddress(packet->getMandatoryTag<MacAddressReq>()->getDestAddress());
            auto userPriorityReq = packet->getTag<UserPriorityReq>();
            if (userPriorityReq != nullptr) {
                // make it a QoS ieee80211MacHeader, and set TID
                ieee80211MacHeader->setType(ST_DATA_WITH_QOS);
                ieee80211MacHeader->setChunkLength(ieee80211MacHeader->getChunkLength() + bit(QOSCONTROL_BITS));
                ieee80211MacHeader->setTid(userPriorityReq->getUserPriority());
            }

            // encapsulate payload
            packet->insertHeader(ieee80211MacHeader);
        }
        break;

        case ENCAP_DECAP_FALSE:
            break;

        default:
            throw cRuntimeError("Unknown encapDecap value: %d", encapDecap);
            break;
    }
}

} // namespace ieee80211

} // namespace inet

