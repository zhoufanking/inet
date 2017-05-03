//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/ieee802/Ieee802LlcHeader_m.h"
#include "inet/linklayer/ieee80211/mac/coordinationfunction/Dcf.h"
#include "inet/linklayer/ieee80211/mac/framesequence/DcfFs.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Mac.h"
#include "inet/linklayer/ieee80211/mac/rateselection/RateSelection.h"
#include "inet/linklayer/ieee80211/mac/recipient/RecipientAckProcedure.h"

namespace inet {
namespace ieee80211 {

Define_Module(Dcf);

void Dcf::initialize(int stage)
{
    ModeSetListener::initialize(stage);
    if (stage == INITSTAGE_LINK_LAYER_2) {
        startRxTimer = new cMessage("startRxTimeout");
        mac = check_and_cast<Ieee80211Mac *>(getContainingNicModule(this)->getSubmodule("mac"));
        dataAndMgmtRateControl = dynamic_cast<IRateControl *>(getSubmodule(("rateControl")));
        tx = check_and_cast<ITx *>(getModuleByPath(par("txModule")));
        rx = check_and_cast<IRx *>(getModuleByPath(par("rxModule")));
        dcfChannelAccess = check_and_cast<IChannelAccess *>(getSubmodule("channelAccess"));
        originatorDataService = check_and_cast<IOriginatorMacDataService *>(getSubmodule(("originatorMacDataService")));
        recipientDataService = check_and_cast<IRecipientMacDataService*>(getSubmodule("recipientMacDataService"));
        recoveryProcedure = check_and_cast<NonQoSRecoveryProcedure *>(getSubmodule("recoveryProcedure"));
        rateSelection = check_and_cast<IRateSelection*>(getSubmodule("rateSelection"));
        pendingQueue = new PendingQueue(par("maxQueueSize"), nullptr, par("prioritizeMulticast") ? PendingQueue::Priority::PRIORITIZE_MULTICAST_OVER_DATA : PendingQueue::Priority::PRIORITIZE_MGMT_OVER_DATA);
        rtsProcedure = new RtsProcedure();
        rtsPolicy = check_and_cast<IRtsPolicy *>(getSubmodule("rtsPolicy"));
        recipientAckProcedure = new RecipientAckProcedure();
        recipientAckPolicy = check_and_cast<IRecipientAckPolicy *>(getSubmodule("recipientAckPolicy"));
        originatorAckPolicy = check_and_cast<IOriginatorAckPolicy *>(getSubmodule("originatorAckPolicy"));
        frameSequenceHandler = new FrameSequenceHandler();
        ackHandler = new AckHandler();
        ctsProcedure = new CtsProcedure();
        ctsPolicy = check_and_cast<ICtsPolicy *>(getSubmodule("ctsPolicy"));
        stationRetryCounters = new StationRetryCounters();
        inProgressFrames = new InProgressFrames(pendingQueue, originatorDataService, ackHandler);
        originatorProtectionMechanism = check_and_cast<OriginatorProtectionMechanism*>(getSubmodule("originatorProtectionMechanism"));
    }
}

void Dcf::handleMessage(cMessage* msg)
{
    if (msg == startRxTimer) {
        if (!isReceptionInProgress())
            frameSequenceHandler->handleStartRxTimeout();
    }
    else
        throw cRuntimeError("Unknown msg type");
}

void Dcf::channelGranted(IChannelAccess *channelAccess)
{
    ASSERT(dcfChannelAccess == channelAccess);
    if (!frameSequenceHandler->isSequenceRunning())
        frameSequenceHandler->startFrameSequence(new DcfFs(), buildContext(), this);
}

void Dcf::processUpperFrame(Packet *packet, const Ptr<Ieee80211DataOrMgmtFrame>& frame)
{
    Enter_Method("processUpperFrame(%s)", frame->getName());
    EV_INFO << "Processing upper frame: " << frame->getName() << endl;
    if (pendingQueue->insert(packet)) {
        EV_INFO << "Frame " << frame->getName() << " has been inserted into the PendingQueue." << endl;
        EV_DETAIL << "Requesting channel" << endl;
        dcfChannelAccess->requestChannel(this);
    }
    else {
        EV_INFO << "Frame " << frame->getName() << " has been dropped because the PendingQueue is full." << endl;
        emit(NF_PACKET_DROP, packet);
        delete packet;
    }
}

void Dcf::transmitControlResponseFrame(Packet *responsePacket, const Ptr<Ieee80211Frame>& responseFrame, Packet *receivedPacket, const Ptr<Ieee80211Frame>& receivedFrame)
{
    responsePacket->insertTrailer(std::make_shared<Ieee80211MacTrailer>());
    const IIeee80211Mode *responseMode = nullptr;
    if (auto rtsFrame = std::dynamic_pointer_cast<Ieee80211RTSFrame>(receivedFrame))
        responseMode = rateSelection->computeResponseCtsFrameMode(responsePacket, rtsFrame);
    else if (auto dataOrMgmtFrame = std::dynamic_pointer_cast<Ieee80211DataOrMgmtFrame>(receivedFrame))
        responseMode = rateSelection->computeResponseAckFrameMode(receivedPacket, dataOrMgmtFrame);
    else
        throw cRuntimeError("Unknown received frame type");
    RateSelection::setFrameMode(responsePacket, responseFrame, responseMode);
    tx->transmitFrame(responsePacket, responseFrame, modeSet->getSifsTime(), this);
    delete responsePacket;
}

void Dcf::processMgmtFrame(Packet *packet, const Ptr<Ieee80211ManagementHeader>& mgmtFrame)
{
    throw cRuntimeError("Unknown management frame");
}

void Dcf::recipientProcessTransmittedControlResponseFrame(const Ptr<Ieee80211Frame>& frame)
{
    if (auto ctsFrame = std::dynamic_pointer_cast<Ieee80211CTSFrame>(frame))
        ctsProcedure->processTransmittedCts(ctsFrame);
    else if (auto ackFrame = std::dynamic_pointer_cast<Ieee80211ACKFrame>(frame))
        recipientAckProcedure->processTransmittedAck(ackFrame);
    else
        throw cRuntimeError("Unknown control response frame");
}

void Dcf::scheduleStartRxTimer(simtime_t timeout)
{
    Enter_Method_Silent();
    scheduleAt(simTime() + timeout, startRxTimer);
}

void Dcf::processLowerFrame(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    Enter_Method_Silent();
    if (frameSequenceHandler->isSequenceRunning()) {
        // TODO: always call processResponses
        if ((!isForUs(frame) && !startRxTimer->isScheduled()) || isForUs(frame)) {
            frameSequenceHandler->processResponse(packet);
        }
        else {
            EV_INFO << "This frame is not for us" << std::endl;
            delete packet;
        }
        cancelEvent(startRxTimer);
    }
    else if (isForUs(frame))
        recipientProcessReceivedFrame(packet, frame);
    else {
        EV_INFO << "This frame is not for us" << std::endl;
        delete packet;
    }
}

void Dcf::transmitFrame(Packet *packet, simtime_t ifs)
{
    const auto& frame = packet->peekHeader<Ieee80211Frame>();
    RateSelection::setFrameMode(packet, frame, rateSelection->computeMode(packet, frame));
    auto pendingPacket = inProgressFrames->getPendingFrameFor(packet);
    auto duration = originatorProtectionMechanism->computeDurationField(packet, frame, pendingPacket, pendingPacket == nullptr ? nullptr : pendingPacket->peekHeader<Ieee80211DataOrMgmtFrame>());
    auto header = packet->removeHeader<Ieee80211Frame>();
    header->setDuration(duration);
    packet->insertHeader(header);
    tx->transmitFrame(packet, packet->peekHeader<Ieee80211Frame>(), ifs, this);
}

/*
 * TODO:  If a PHY-RXSTART.indication primitive does not occur during the ACKTimeout interval,
 * the STA concludes that the transmission of the MPDU has failed, and this STA shall invoke its
 * backoff procedure **upon expiration of the ACKTimeout interval**.
 */

void Dcf::frameSequenceFinished()
{
    dcfChannelAccess->releaseChannel(this);
    if (hasFrameToTransmit())
        dcfChannelAccess->requestChannel(this);
    mac->sendDownPendingRadioConfigMsg(); // TODO: review
}

bool Dcf::isReceptionInProgress()
{
    return rx->isReceptionInProgress();
}

void Dcf::recipientProcessReceivedFrame(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    if (auto dataOrMgmtFrame = std::dynamic_pointer_cast<Ieee80211DataOrMgmtFrame>(frame))
        recipientAckProcedure->processReceivedFrame(packet, dataOrMgmtFrame, recipientAckPolicy, this);
    if (auto dataFrame = std::dynamic_pointer_cast<Ieee80211DataFrame>(frame))
        sendUp(recipientDataService->dataFrameReceived(packet, dataFrame));
    else if (auto mgmtFrame = std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame))
        sendUp(recipientDataService->managementFrameReceived(packet, mgmtFrame));
    else { // TODO: else if (auto ctrlFrame = dynamic_cast<Ieee80211ControlFrame*>(frame))
        sendUp(recipientDataService->controlFrameReceived(packet, frame));
        recipientProcessControlFrame(packet, frame);
    }
}

void Dcf::sendUp(const std::vector<Packet*>& completeFrames)
{
    for (auto frame : completeFrames)
        mac->sendUp(frame);
}

void Dcf::recipientProcessControlFrame(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    if (auto rtsFrame = std::dynamic_pointer_cast<Ieee80211RTSFrame>(frame))
        ctsProcedure->processReceivedRts(packet, rtsFrame, ctsPolicy, this);
    else
        throw cRuntimeError("Unknown control frame");
}

FrameSequenceContext* Dcf::buildContext()
{
    auto nonQoSContext = new NonQoSContext(originatorAckPolicy);
    return new FrameSequenceContext(mac->getAddress(), modeSet, inProgressFrames, rtsProcedure, rtsPolicy, nonQoSContext, nullptr);
}

void Dcf::transmissionComplete(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    if (frameSequenceHandler->isSequenceRunning())
        frameSequenceHandler->transmissionComplete();
    else
        recipientProcessTransmittedControlResponseFrame(frame);
    delete packet;
}

bool Dcf::hasFrameToTransmit()
{
    return !pendingQueue->isEmpty() || inProgressFrames->hasInProgressFrames();
}

void Dcf::originatorProcessRtsProtectionFailed(Packet *packet)
{
    EV_INFO << "RTS frame transmission failed\n";
    auto protectedFrame = packet->peekHeader<Ieee80211DataOrMgmtFrame>();
    recoveryProcedure->rtsFrameTransmissionFailed(protectedFrame, stationRetryCounters);
    if (recoveryProcedure->isRtsFrameRetryLimitReached(packet, protectedFrame)) {
        recoveryProcedure->retryLimitReached(packet, protectedFrame);
        inProgressFrames->dropFrame(packet);
        emit(NF_PACKET_DROP, packet);
        emit(NF_LINK_BREAK, packet);
        delete packet;
    }
}

void Dcf::originatorProcessTransmittedFrame(Packet *packet)
{
    auto transmittedFrame = packet->peekHeader<Ieee80211Frame>();
    if (auto dataOrMgmtFrame = std::dynamic_pointer_cast<Ieee80211DataOrMgmtFrame>(transmittedFrame)) {
        if (originatorAckPolicy->isAckNeeded(dataOrMgmtFrame)) {
            ackHandler->processTransmittedDataOrMgmtFrame(dataOrMgmtFrame);
        }
        else if (dataOrMgmtFrame->getReceiverAddress().isMulticast()) {
            recoveryProcedure->multicastFrameTransmitted(stationRetryCounters);
            inProgressFrames->dropFrame(packet);
        }
    }
    else if (auto rtsFrame = std::dynamic_pointer_cast<Ieee80211RTSFrame>(transmittedFrame))
        rtsProcedure->processTransmittedRts(rtsFrame);
}

void Dcf::originatorProcessReceivedFrame(Packet *packet, Packet *lastTransmittedPacket)
{
    auto frame = packet->peekHeader<Ieee80211Frame>();
    auto lastTransmittedFrame = lastTransmittedPacket->peekHeader<Ieee80211Frame>();
    if (frame->getType() == ST_ACK) {
        auto lastTransmittedDataOrMgmtFrame = std::dynamic_pointer_cast<Ieee80211DataOrMgmtFrame>(lastTransmittedFrame);
        if (dataAndMgmtRateControl) {
            int retryCount;
            if (lastTransmittedFrame->getRetry())
                retryCount = recoveryProcedure->getRetryCount(packet, lastTransmittedDataOrMgmtFrame);
            else
                retryCount = 0;
            dataAndMgmtRateControl->frameTransmitted(packet, retryCount, true, false);
        }
        recoveryProcedure->ackFrameReceived(packet, lastTransmittedDataOrMgmtFrame, stationRetryCounters);
        ackHandler->processReceivedAck(std::dynamic_pointer_cast<Ieee80211ACKFrame>(frame), lastTransmittedDataOrMgmtFrame);
        inProgressFrames->dropFrame(lastTransmittedPacket);
    }
    else if (frame->getType() == ST_RTS)
        ; // void
    else if (frame->getType() == ST_CTS)
        recoveryProcedure->ctsFrameReceived(stationRetryCounters);
    else
        throw cRuntimeError("Unknown frame type");
    delete packet;
}

void Dcf::originatorProcessFailedFrame(Packet *packet)
{
    EV_INFO << "Data/Mgmt frame transmission failed\n";
    const auto& failedFrame = packet->peekHeader<Ieee80211DataOrMgmtFrame>();
    ASSERT(failedFrame->getType() != ST_DATA_WITH_QOS);
    ASSERT(ackHandler->getAckStatus(failedFrame) == AckHandler::Status::WAITING_FOR_ACK);
    recoveryProcedure->dataOrMgmtFrameTransmissionFailed(packet, failedFrame, stationRetryCounters);
    bool retryLimitReached = recoveryProcedure->isRetryLimitReached(packet, failedFrame);
    if (dataAndMgmtRateControl) {
        int retryCount = recoveryProcedure->getRetryCount(packet, failedFrame);
        dataAndMgmtRateControl->frameTransmitted(packet, retryCount, false, retryLimitReached);
    }
    ackHandler->processFailedFrame(failedFrame);
    if (retryLimitReached) {
        recoveryProcedure->retryLimitReached(packet, failedFrame);
        inProgressFrames->dropFrame(packet);
        // KLUDGE: removed headers and trailers to allow higher layer protocols to process the packet
        packet->popHeader<Ieee80211DataOrMgmtFrame>();
        const auto& nextHeader = packet->peekHeader();
        if (std::dynamic_pointer_cast<Ieee802LlcHeader>(nextHeader))
            packet->popHeader<Ieee802LlcHeader>();
        packet->popTrailer<Ieee80211MacTrailer>();
        emit(NF_PACKET_DROP, packet);
        emit(NF_LINK_BREAK, packet);
        delete packet;
    }
    else {
        auto h = packet->removeHeader<Ieee80211DataOrMgmtFrame>();
        h->setRetry(true);
        packet->insertHeader(h);
    }
}

bool Dcf::isForUs(const Ptr<Ieee80211Frame>& frame) const
{
    return frame->getReceiverAddress() == mac->getAddress() || (frame->getReceiverAddress().isMulticast() && !isSentByUs(frame));
}

bool Dcf::isSentByUs(const Ptr<Ieee80211Frame>& frame) const
{
    // FIXME:
    // Check the roles of the Addr3 field when aggregation is applied
    // Table 8-19—Address field contents
    if (auto dataOrMgmtFrame = std::dynamic_pointer_cast<Ieee80211DataOrMgmtFrame>(frame))
        return dataOrMgmtFrame->getAddress3() == mac->getAddress();
    else
        return false;
}

void Dcf::corruptedFrameReceived()
{
    if (frameSequenceHandler->isSequenceRunning()) {
        if (!startRxTimer->isScheduled())
            frameSequenceHandler->handleStartRxTimeout();
    }
}

Dcf::~Dcf()
{
    cancelAndDelete(startRxTimer);
    delete pendingQueue;
    delete inProgressFrames;
    delete rtsProcedure;
    delete recipientAckProcedure;
    delete ackHandler;
    delete stationRetryCounters;
    delete frameSequenceHandler;
    delete ctsProcedure;
}

} // namespace ieee80211
} // namespace inet

