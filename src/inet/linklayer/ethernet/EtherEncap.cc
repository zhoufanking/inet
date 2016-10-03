/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

#include "inet/linklayer/ethernet/EtherEncap.h"

#include "inet/applications/common/SocketTag_m.h"
#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/linklayer/common/EtherTypeTag_m.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/linklayer/common/Ieee802SapTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MACAddressTag_m.h"
#include "inet/linklayer/ethernet/EtherFrame.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

namespace inet {

Define_Module(EtherEncap);

simsignal_t EtherEncap::encapPkSignal = registerSignal("encapPk");
simsignal_t EtherEncap::decapPkSignal = registerSignal("decapPk");
simsignal_t EtherEncap::pauseSentSignal = registerSignal("pauseSent");

void EtherEncap::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        seqNum = 0;
        WATCH(seqNum);
        totalFromHigherLayer = totalFromMAC = totalPauseSent = droppedUnknownDest = 0;
        useSNAP = par("useSNAP").boolValue();
        WATCH(totalFromHigherLayer);
        WATCH(totalFromMAC);
        WATCH(totalPauseSent);
//FIXME        WATCH(dsapToSocketIds);
        WATCH(droppedUnknownDest);
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        // lifecycle
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isUp = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
        if (isUp)
            start();
    }
}

void EtherEncap::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("lowerLayerIn")) {
        EV_INFO << "Received " << msg << " from lower layer." << endl;
        processFrameFromMAC(check_and_cast<EtherFrame *>(msg));
    }
    else {
        EV_INFO << "Received " << msg << " from upper layer." << endl;
        // from higher layer
        switch (msg->getKind()) {
            case IEEE802CTRL_DATA:
            case 0:    // default message kind (0) is also accepted
                processPacketFromHigherLayer(PK(msg));
                break;

            case IEEE802CTRL_REGISTER_DSAP:
                // higher layer registers itself
                handleRegisterSAP(msg);
                break;

            case IEEE802CTRL_DEREGISTER_DSAP:
                // higher layer deregisters itself
                handleDeregisterSAP(msg);
                break;

            case IEEE802CTRL_SENDPAUSE:
                // higher layer want MAC to send PAUSE frame
                handleSendPause(msg);
                break;

            default:
                throw cRuntimeError("Received message `%s' with unknown message kind %d", msg->getName(), msg->getKind());
        }
    }
}

void EtherEncap::refreshDisplay() const
{
    char buf[180];
    sprintf(buf, "passed up: %ld\nsent: %ld", totalFromMAC, totalFromHigherLayer);
    if (droppedUnknownDest > 0) {
        sprintf(buf + strlen(buf), "\ndropped (wrong DSAP): %ld", droppedUnknownDest);
    }

    getDisplayString().setTagArg("t", 0, buf);
}

void EtherEncap::processPacketFromHigherLayer(cPacket *msg)
{
    if (msg->getByteLength() > MAX_ETHERNET_DATA_BYTES)
        throw cRuntimeError("packet from higher layer (%d bytes) exceeds maximum Ethernet payload length (%d)", (int)msg->getByteLength(), MAX_ETHERNET_DATA_BYTES);

    totalFromHigherLayer++;
    emit(encapPkSignal, msg);

    // Creates MAC header information and encapsulates received higher layer data
    // with this information and transmits resultant frame to lower layer

    // create Ethernet frame, fill it in from Ieee802Ctrl and encapsulate msg in it
    EV_DETAIL << "Encapsulating higher layer packet `" << msg->getName() << "' for MAC\n";

    auto macAddressReq = msg->getMandatoryTag<MacAddressReq>();
    auto ieee802SapReq = msg->getTag<Ieee802SapReq>();
    auto etherTypeTag = msg->getTag<EtherTypeReq>();
    EtherFrame *frame = nullptr;

    //FIXME get the kind of ethernet frame from Tags instead of module parameter
    if (useSNAP) {
        auto *snapFrame = new EtherFrameWithSNAP(msg->getName());

        snapFrame->setOrgCode(0);
        if (etherTypeTag)
            snapFrame->setLocalcode(etherTypeTag->getEtherType());
        frame = snapFrame;
    }
    else if (ieee802SapReq != nullptr){
        auto *llcFrame = new EtherFrameWithLLC(msg->getName());

        llcFrame->setControl(0);
        llcFrame->setSsap(ieee802SapReq->getSsap());
        llcFrame->setDsap(ieee802SapReq->getDsap());

        frame = llcFrame;
    }
    else {
        auto *eth2Frame = new EthernetIIFrame(msg->getName());

        if (etherTypeTag)
            eth2Frame->setEtherType(etherTypeTag->getEtherType());
        frame = eth2Frame;
    }

    frame->setSrc(macAddressReq->getSrcAddress());    // if blank, will be filled in by MAC
    frame->setDest(macAddressReq->getDestAddress());

    ASSERT(frame->getByteLength() > 0); // length comes from msg file

    frame->encapsulate(msg);
    if (frame->getByteLength() < MIN_ETHERNET_FRAME_BYTES)
        frame->setByteLength(MIN_ETHERNET_FRAME_BYTES); // "padding"

    EV_INFO << "Sending " << frame << " to lower layer.\n";
    send(frame, "lowerLayerOut");
}

void EtherEncap::processFrameFromMAC(EtherFrame *frame)
{
    // decapsulate and attach control info
    cPacket *higherlayermsg = frame->decapsulate();
    delete higherlayermsg->removeTag<DispatchProtocolReq>();

    // add Ieee802Ctrl to packet
    auto macAddressInd = higherlayermsg->ensureTag<MacAddressInd>();
    macAddressInd->setSrcAddress(frame->getSrc());
    macAddressInd->setDestAddress(frame->getDest());

    int etherType = -1;
    int dSap = -1;
    if (auto eth2frame = dynamic_cast<EthernetIIFrame *>(frame)) {
        etherType = eth2frame->getEtherType();
    }
    else if (EtherFrameWithLLC *llcFrame = dynamic_cast<EtherFrameWithLLC *>(frame)) {
        dSap = llcFrame->getDsap();
        auto ieee802SapInd = higherlayermsg->ensureTag<Ieee802SapInd>();
        ieee802SapInd->setSsap(llcFrame->getSsap());
        ieee802SapInd->setDsap(llcFrame->getDsap());
        if (auto snapFrame = dynamic_cast<EtherFrameWithSNAP *>(llcFrame)) {
            if (llcFrame->getSsap() == 0xAA && llcFrame->getDsap() == 0xAA && llcFrame->getControl() == 0)
                etherType = snapFrame->getLocalcode();
        }
    }
    int protocolId = -1;
    if (etherType != -1) {
        higherlayermsg->ensureTag<EtherTypeInd>()->setEtherType(etherType);
        if (auto protocol = ProtocolGroup::ethertype.findProtocol(etherType)) {
            higherlayermsg->ensureTag<DispatchProtocolReq>()->setProtocol(protocol);
            protocolId = protocol->getId();
        }
    }

    totalFromMAC++;
    emit(decapPkSignal, higherlayermsg);
    EV_DETAIL << "Decapsulating frame `" << frame->getName() << "', contained packet is `" << higherlayermsg->getName() << "'\n";

    bool sent = false;
    if (dSap != -1) {
        for (auto elem: dsapToSocketIds) {
            if (elem.dsap == dSap) {
                cPacket *packetCopy = higherlayermsg->dup();
                packetCopy->ensureTag<SocketInd>()->setSocketId(elem.socketId);
                EV_INFO << "Sending " << packetCopy << " to upper layer for socket " << elem.socketId << ".\n";
                send(packetCopy, "upperLayerOut");
                sent = true;
            }
        }
    }
    if (!sent && mapping.findOutputGateForProtocol(protocolId)) {
        // pass up to higher layers.
        EV_INFO << "Sending " << higherlayermsg << " to upper layer, etherType=" << etherType <<".\n";
        send(higherlayermsg, "upperLayerOut");
        sent = true;
    }
    if (!sent) {
        EV << "No higher layer registered for DSAP=" << dSap << " and etherType not specified, discarding frame `" << frame->getName() << "'\n";
        droppedUnknownDest++;
        delete higherlayermsg;
        delete frame;
        return;
    }
    delete frame;
}

void EtherEncap::handleRegisterSAP(cMessage *msg)
{
    Ieee802RegisterDsapCommand *etherctrl = dynamic_cast<Ieee802RegisterDsapCommand *>(msg->getControlInfo());
    if (!etherctrl)
        throw cRuntimeError("packet `%s' from higher layer received without Ieee802RegisterDsapCommand", msg->getName());

    DsapAndSocketId newSap(etherctrl->getDsap(), msg->getMandatoryTag<SocketReq>()->getSocketId());

    EV << "Registering higher layer with DSAP=" << newSap.dsap << " for socket " << newSap.socketId << endl;

    for (auto &elem: dsapToSocketIds) {
        if (elem == newSap)
            throw cRuntimeError("DSAP=%d already registered with socket %d", newSap.dsap, newSap.socketId);
    }

    dsapToSocketIds.push_back(newSap);
    delete msg;
}

void EtherEncap::handleDeregisterSAP(cMessage *msg)
{
    Ieee802DeregisterDsapCommand *etherctrl = dynamic_cast<Ieee802DeregisterDsapCommand *>(msg->getControlInfo());
    if (!etherctrl)
        throw cRuntimeError("packet `%s' from higher layer received without Ieee802DeregisterDsapCommand", msg->getName());

    DsapAndSocketId oldSap(etherctrl->getDsap(), msg->getMandatoryTag<SocketReq>()->getSocketId());

    EV << "Deregistering higher layer with DSAP=" << oldSap.dsap << " and socket " << oldSap.socketId << endl;

    // delete from table (don't care if it's not in there)
    for (auto it = dsapToSocketIds.begin(); it != dsapToSocketIds.end(); ++it) {
        if (*it == oldSap) {
            dsapToSocketIds.erase(it);
            delete msg;
            return;
        }
    }
    throw cRuntimeError("DSAP=%d did not registered with socket %d", oldSap.dsap, oldSap.socketId);
}

void EtherEncap::handleSendPause(cMessage *msg)
{
    Ieee802PauseCommand *etherctrl = dynamic_cast<Ieee802PauseCommand *>(msg->removeControlInfo());
    if (!etherctrl)
        throw cRuntimeError("PAUSE command `%s' from higher layer received without Ieee802PauseCommand controlinfo", msg->getName());
    MACAddress dest = etherctrl->getDestinationAddress();
    int pauseUnits = etherctrl->getPauseUnits();
    delete etherctrl;

    EV_DETAIL << "Creating and sending PAUSE frame, with duration = " << pauseUnits << " units\n";

    // create Ethernet frame
    char framename[40];
    sprintf(framename, "pause-%d-%d", getId(), seqNum++);
    EtherPauseFrame *frame = new EtherPauseFrame(framename);
    frame->setPauseTime(pauseUnits);
    if (dest.isUnspecified())
        dest = MACAddress::MULTICAST_PAUSE_ADDRESS;
    frame->setDest(dest);
    frame->setByteLength(ETHER_PAUSE_COMMAND_PADDED_BYTES);

    EV_INFO << "Sending " << frame << " to lower layer.\n";
    send(frame, "lowerLayerOut");
    delete msg;

    emit(pauseSentSignal, pauseUnits);
    totalPauseSent++;
}

bool EtherEncap::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_NETWORK_LAYER)
            start();
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_NETWORK_LAYER)
            stop();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH)
            stop();
    }
    return true;
}

void EtherEncap::handleRegisterProtocol(const Protocol& protocol, cGate *gate)
{
    Enter_Method("handleRegisterProtocol");
    mapping.addProtocolMapping(ProtocolGroup::ipprotocol.getProtocolNumber(&protocol), gate->getIndex());
}

void EtherEncap::start()
{
    dsapToSocketIds.clear();
    isUp = true;
}

void EtherEncap::stop()
{
    dsapToSocketIds.clear();
    isUp = false;
}

} // namespace inet

