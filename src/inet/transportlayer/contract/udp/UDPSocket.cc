//
// Copyright (C) 2005,2011 Andras Varga
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

#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/linklayer/common/SimpleLinkLayerControlInfo.h"
#ifdef WITH_IPv4
#include "inet/networklayer/ipv4/IPv4InterfaceData.h"
#endif // ifdef WITH_IPv4

namespace inet {

UDPSocket::UDPSocket()
{
    // don't allow user-specified sockIds because they may conflict with
    // automatically assigned ones.
    sockId = generateSocketId();
    gateToUdp = nullptr;
}

int UDPSocket::generateSocketId()
{
    return getEnvir()->getUniqueNumber();
}

void UDPSocket::sendToUDP(cMessage *msg)
{
    if (!gateToUdp)
        throw cRuntimeError("UDPSocket: setOutputGate() must be invoked before socket can be used");

    cObject *ctrl = msg->getControlInfo();
    EV_TRACE << "UDPSocket: Send (" << msg->getClassName() << ")" << msg->getFullName();
    if (ctrl)
        EV_TRACE << "  control info: (" << ctrl->getClassName() << ")" << ctrl->getFullName();
    EV_TRACE << endl;

    check_and_cast<cSimpleModule *>(gateToUdp->getOwnerModule())->send(msg, gateToUdp);
}

void UDPSocket::bind(int localPort)
{
    bind(L3Address(), localPort);
}

void UDPSocket::bind(L3Address localAddr, int localPort)
{
    if (localPort < -1 || localPort > 65535) // -1: ephemeral port
        throw cRuntimeError("UDPSocket::bind(): invalid port number %d", localPort);

    cMessage *msg = new cMessage("BIND", UDP_C_BIND);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPBindCommand *ctrl = msg->ensureTag<UDPBindCommand>();
    ctrl->setLocalAddr(localAddr);
    ctrl->setLocalPort(localPort);
    sendToUDP(msg);
}

void UDPSocket::connect(L3Address addr, int port)
{
    if (addr.isUnspecified())
        throw cRuntimeError("UDPSocket::connect(): unspecified remote address");
    if (port <= 0 || port > 65535)
        throw cRuntimeError("UDPSocket::connect(): invalid remote port number %d", port);

    cMessage *msg = new cMessage("CONNECT", UDP_C_CONNECT);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPConnectCommand *ctrl = msg->ensureTag<UDPConnectCommand>();
    ctrl->setRemoteAddr(addr);
    ctrl->setRemotePort(port);
    sendToUDP(msg);
}

void UDPSocket::sendTo(cPacket *pk, L3Address destAddr, int destPort, const SendOptions *options)
{
    pk->setKind(UDP_C_DATA);
    UDPSocketIdRequestTag *sid = pk->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSendCommand *ctrl = pk->ensureTag<UDPSendCommand>();
    ctrl->setDestAddr(destAddr);
    ctrl->setDestPort(destPort);
    if (options) {
        ctrl->setSrcAddr(options->srcAddr);
        ctrl->setInterfaceId(options->outInterfaceId);
    }
    sendToUDP(pk);
}

void UDPSocket::send(cPacket *pk)
{
    pk->setKind(UDP_C_DATA);
    UDPSocketIdRequestTag *sid = pk->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSendCommand *ctrl = pk->ensureTag<UDPSendCommand>();
    sendToUDP(pk);
}

void UDPSocket::close()
{
    cMessage *msg = new cMessage("CLOSE", UDP_C_CLOSE);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPCloseCommand *ctrl = msg->ensureTag<UDPCloseCommand>();
    sendToUDP(msg);
}

void UDPSocket::setBroadcast(bool broadcast)
{
    cMessage *msg = new cMessage("SetBroadcast", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetBroadcastCommand *ctrl = msg->ensureTag<UDPSetBroadcastCommand>();
    ctrl->setBroadcast(broadcast);
    sendToUDP(msg);
}

void UDPSocket::setTimeToLive(int ttl)
{
    cMessage *msg = new cMessage("SetTTL", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetTimeToLiveCommand *ctrl = msg->ensureTag<UDPSetTimeToLiveCommand>();
    ctrl->setTtl(ttl);
    sendToUDP(msg);
}

void UDPSocket::setTypeOfService(unsigned char tos)
{
    cMessage *msg = new cMessage("SetTOS", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetTypeOfServiceCommand *ctrl = msg->ensureTag<UDPSetTypeOfServiceCommand>();
    ctrl->setTos(tos);
    sendToUDP(msg);
}

void UDPSocket::setMulticastOutputInterface(int interfaceId)
{
    cMessage *msg = new cMessage("SetMulticastOutputIf", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetMulticastInterfaceCommand *ctrl = msg->ensureTag<UDPSetMulticastInterfaceCommand>();
    ctrl->setInterfaceId(interfaceId);
    sendToUDP(msg);
}

void UDPSocket::setMulticastLoop(bool value)
{
    cMessage *msg = new cMessage("SetMulticastLoop", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetMulticastLoopCommand *ctrl = msg->ensureTag<UDPSetMulticastLoopCommand>();
    ctrl->setLoop(value);
    sendToUDP(msg);
}

void UDPSocket::setReuseAddress(bool value)
{
    cMessage *msg = new cMessage("SetReuseAddress", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetReuseAddressCommand *ctrl = msg->ensureTag<UDPSetReuseAddressCommand>();
    ctrl->setReuseAddress(value);
    sendToUDP(msg);
}

void UDPSocket::joinMulticastGroup(const L3Address& multicastAddr, int interfaceId)
{
    cMessage *msg = new cMessage("JoinMulticastGroups", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPJoinMulticastGroupsCommand *ctrl = msg->ensureTag<UDPJoinMulticastGroupsCommand>();
    ctrl->setMulticastAddrArraySize(1);
    ctrl->setMulticastAddr(0, multicastAddr);
    ctrl->setInterfaceIdArraySize(1);
    ctrl->setInterfaceId(0, interfaceId);
    sendToUDP(msg);
}

void UDPSocket::joinLocalMulticastGroups(MulticastGroupList mgl)
{
    if (mgl.size() > 0) {
        cMessage *msg = new cMessage("JoinMulticastGroups", UDP_C_SETOPTION);
        UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
        sid->setSockId(sockId);
        UDPJoinMulticastGroupsCommand *ctrl = msg->ensureTag<UDPJoinMulticastGroupsCommand>();
        ctrl->setMulticastAddrArraySize(mgl.size());
        ctrl->setInterfaceIdArraySize(mgl.size());

        for (unsigned int j = 0; j < mgl.size(); ++j) {
            ctrl->setMulticastAddr(j, mgl[j].multicastAddr);
            ctrl->setInterfaceId(j, mgl[j].interfaceId);
        }

        sendToUDP(msg);
    }
}

void UDPSocket::leaveMulticastGroup(const L3Address& multicastAddr)
{
    cMessage *msg = new cMessage("LeaveMulticastGroups", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPLeaveMulticastGroupsCommand *ctrl = msg->ensureTag<UDPLeaveMulticastGroupsCommand>();
    ctrl->setMulticastAddrArraySize(1);
    ctrl->setMulticastAddr(0, multicastAddr);
    sendToUDP(msg);
}

void UDPSocket::leaveLocalMulticastGroups(MulticastGroupList mgl)
{
    if (mgl.size() > 0) {
        cMessage *msg = new cMessage("LeaveMulticastGroups", UDP_C_SETOPTION);
        UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
        sid->setSockId(sockId);
        UDPLeaveMulticastGroupsCommand *ctrl = msg->ensureTag<UDPLeaveMulticastGroupsCommand>();
        ctrl->setMulticastAddrArraySize(mgl.size());

        for (unsigned int j = 0; j < mgl.size(); ++j) {
            ctrl->setMulticastAddr(j, mgl[j].multicastAddr);
        }

        sendToUDP(msg);
    }
}

void UDPSocket::blockMulticastSources(int interfaceId, const L3Address& multicastAddr, const std::vector<L3Address>& sourceList)
{
    cMessage *msg = new cMessage("BlockMulticastSources", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPBlockMulticastSourcesCommand *ctrl = msg->ensureTag<UDPBlockMulticastSourcesCommand>();
    ctrl->setInterfaceId(interfaceId);
    ctrl->setMulticastAddr(multicastAddr);
    ctrl->setSourceListArraySize(sourceList.size());
    for (int i = 0; i < (int)sourceList.size(); ++i)
        ctrl->setSourceList(i, sourceList[i]);
    sendToUDP(msg);
}

void UDPSocket::unblockMulticastSources(int interfaceId, const L3Address& multicastAddr, const std::vector<L3Address>& sourceList)
{
    cMessage *msg = new cMessage("UnblockMulticastSources", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPUnblockMulticastSourcesCommand *ctrl = msg->ensureTag<UDPUnblockMulticastSourcesCommand>();
    ctrl->setInterfaceId(interfaceId);
    ctrl->setMulticastAddr(multicastAddr);
    ctrl->setSourceListArraySize(sourceList.size());
    for (int i = 0; i < (int)sourceList.size(); ++i)
        ctrl->setSourceList(i, sourceList[i]);
    sendToUDP(msg);
}

void UDPSocket::joinMulticastSources(int interfaceId, const L3Address& multicastAddr, const std::vector<L3Address>& sourceList)
{
    cMessage *msg = new cMessage("JoinMulticastSources", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPJoinMulticastSourcesCommand *ctrl = msg->ensureTag<UDPJoinMulticastSourcesCommand>();
    ctrl->setInterfaceId(interfaceId);
    ctrl->setMulticastAddr(multicastAddr);
    ctrl->setSourceListArraySize(sourceList.size());
    for (int i = 0; i < (int)sourceList.size(); ++i)
        ctrl->setSourceList(i, sourceList[i]);
    sendToUDP(msg);
}

void UDPSocket::leaveMulticastSources(int interfaceId, const L3Address& multicastAddr, const std::vector<L3Address>& sourceList)
{
    cMessage *msg = new cMessage("LeaveMulticastSources", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPLeaveMulticastSourcesCommand *ctrl = msg->ensureTag<UDPLeaveMulticastSourcesCommand>();
    ctrl->setInterfaceId(interfaceId);
    ctrl->setMulticastAddr(multicastAddr);
    ctrl->setSourceListArraySize(sourceList.size());
    for (int i = 0; i < (int)sourceList.size(); ++i)
        ctrl->setSourceList(i, sourceList[i]);
    sendToUDP(msg);
}

void UDPSocket::setMulticastSourceFilter(int interfaceId, const L3Address& multicastAddr,
        UDPSourceFilterMode filterMode, const std::vector<L3Address>& sourceList)
{
    cMessage *msg = new cMessage("SetMulticastSourceFilter", UDP_C_SETOPTION);
    UDPSocketIdRequestTag *sid = msg->ensureTag<UDPSocketIdRequestTag>();
    sid->setSockId(sockId);
    UDPSetMulticastSourceFilterCommand *ctrl = msg->ensureTag<UDPSetMulticastSourceFilterCommand>();
    ctrl->setInterfaceId(interfaceId);
    ctrl->setMulticastAddr(multicastAddr);
    ctrl->setFilterMode(filterMode);
    ctrl->setSourceListArraySize(sourceList.size());
    for (int i = 0; i < (int)sourceList.size(); ++i)
        ctrl->setSourceList(i, sourceList[i]);
    sendToUDP(msg);
}

std::string UDPSocket::getReceivedPacketInfo(cPacket *pk)
{
    UDPDataIndication *ctrl = pk->getMandatoryTag<UDPDataIndication>();
    InterfaceIdIndicationTag *idCtrl = pk->getMandatoryTag<InterfaceIdIndicationTag>();

    L3Address srcAddr = ctrl->getSrcAddr();
    L3Address destAddr = ctrl->getDestAddr();
    int srcPort = ctrl->getSrcPort();
    int destPort = ctrl->getDestPort();
    int interfaceID = idCtrl->getInterfaceId();
    int ttl = ctrl->getTtl();
    int tos = ctrl->getTypeOfService();

    std::stringstream os;
    os << pk << " (" << pk->getByteLength() << " bytes) ";
    os << srcAddr << ":" << srcPort << " --> " << destAddr << ":" << destPort;
    os << " TTL=" << ttl << " ToS=" << tos << " on ifID=" << interfaceID;
    return os.str();
}

} // namespace inet

