//
// Copyright (C) 2016 OpenSim Ltd.
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
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/common/ModuleAccess.h"
#include "inet/common/NotifierConsts.h"
#include "inet/linklayer/ieee80211/mac/contract/IRateControl.h"
#include "inet/linklayer/ieee80211/mac/rateselection/RateSelection.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211ModeSet.h"
#include "inet/physicallayer/ieee80211/mode/IIeee80211Mode.h"
#include "inet/physicallayer/ieee80211/packetlevel/Ieee80211Tag_m.h"

namespace inet {
namespace ieee80211 {

Define_Module(RateSelection);

void RateSelection::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        getContainingNicModule(this)->subscribe(NF_MODESET_CHANGED, this);
    }
    else if (stage == INITSTAGE_LINK_LAYER_2) {
        dataOrMgmtRateControl = dynamic_cast<IRateControl*>(getModuleByPath(par("rateControlModule")));
        double multicastFrameBitrate = par("multicastFrameBitrate");
        multicastFrameMode = (multicastFrameBitrate == -1) ? nullptr : modeSet->getMode(bps(multicastFrameBitrate));
        double dataFrameBitrate = par("dataFrameBitrate");
        dataFrameMode = (dataFrameBitrate == -1) ? nullptr : modeSet->getMode(bps(dataFrameBitrate));
        double mgmtFrameBitrate = par("mgmtFrameBitrate");
        mgmtFrameMode = (mgmtFrameBitrate == -1) ? nullptr : modeSet->getMode(bps(mgmtFrameBitrate));
        double controlFrameBitrate = par("controlFrameBitrate");
        controlFrameMode = (controlFrameBitrate == -1) ? nullptr : modeSet->getMode(bps(controlFrameBitrate));
        double responseAckFrameBitrate = par("responseAckFrameBitrate");
        responseAckFrameMode = (responseAckFrameBitrate == -1) ? nullptr : modeSet->getMode(bps(responseAckFrameBitrate));
        double responseCtsFrameBitrate = par("responseCtsFrameBitrate");
        responseCtsFrameMode = (responseCtsFrameBitrate == -1) ? nullptr : modeSet->getMode(bps(responseCtsFrameBitrate));
        fastestMandatoryMode = modeSet->getFastestMandatoryMode();
    }
}

const IIeee80211Mode* RateSelection::getMode(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    auto modeReqTag = packet->getTag<Ieee80211ModeReq>();
    if (modeReqTag)
        return modeReqTag->getMode();
    auto modeIndTag = packet->getTag<Ieee80211ModeInd>();
    if (modeIndTag)
        return modeIndTag->getMode();
    throw cRuntimeError("Missing mode");
}

//
// In order to allow the transmitting STA to calculate the contents of the Duration/ID field, the responding STA
// shall transmit its Control Response frame (either CTS or ACK) at the same rate as the immediately previous
// frame in the frame exchange sequence (as defined in 9.7), if this rate belongs to the PHY mandatory rates, or
// else at the highest possible rate belonging to the PHY rates in the BSSBasicRateSet.
//
const IIeee80211Mode* RateSelection::computeResponseAckFrameMode(Packet *packet, const Ptr<Ieee80211DataOrMgmtFrame>& dataOrMgmtFrame)
{
    if (responseAckFrameMode)
        return responseAckFrameMode;
    else {
        auto mode = getMode(packet, dataOrMgmtFrame);
        ASSERT(modeSet->containsMode(mode));
        return  modeSet->getIsMandatory(mode) ? mode : fastestMandatoryMode; // TODO: BSSBasicRateSet
    }
}

const IIeee80211Mode* RateSelection::computeResponseCtsFrameMode(Packet *packet, const Ptr<Ieee80211RTSFrame>& rtsFrame)
{
    if (responseCtsFrameMode)
        return responseCtsFrameMode;
    else {
        auto mode = getMode(packet, rtsFrame);
        ASSERT(modeSet->containsMode(mode));
        return modeSet->getIsMandatory(mode) ? mode : fastestMandatoryMode; // TODO: BSSBasicRateSet
    }
}

// 802.11-1999 Std.
//
// All frames with multicast and broadcast RA shall be transmitted at one of the rates included in the
// BSSBasicRateSet, regardless of their type.
//
// TODO: Data and/or management MPDUs with a unicast immediate address shall be sent on any supported data rate
// selected by the rate switching mechanism (whose output is an internal MAC variable called MACCurrentRate,
// defined in units of 500 kbit/s, which is used for calculating the Duration/ID field of each frame). A STA shall
// not transmit at a rate that is known not to be supported by the destination STA, as reported in the supported
// rates element in the management frames. For frames of type Data+CF-ACK, Data+CF-Poll+CF-ACK, and CF-
// Poll+CF-ACK, the rate chosen to transmit the frame must be supported by both the addressed recipient STA
// and the STA to which the ACK is intended.
//
const IIeee80211Mode* RateSelection::computeDataOrMgmtFrameMode(const Ptr<Ieee80211DataOrMgmtFrame>& dataOrMgmtFrame)
{
    if (dataOrMgmtFrame->getReceiverAddress().isMulticast() && multicastFrameMode)
        return multicastFrameMode;
    if (std::dynamic_pointer_cast<Ieee80211DataFrame>(dataOrMgmtFrame) && dataFrameMode)
        return dataFrameMode;
    if (std::dynamic_pointer_cast<Ieee80211ManagementHeader>(dataOrMgmtFrame) && mgmtFrameMode)
        return mgmtFrameMode;
    if (dataOrMgmtRateControl)
        return dataOrMgmtRateControl->getRate();
    else
        return fastestMandatoryMode;
}

// 802.11-1999 Std.
//
// All Control frames shall be transmitted at one of the rates in the BSSBasicRateSet
// (see 10.3.10.1), or at one of the rates in the PHY mandatory rate set so they will
// be understood by all STAs.
//
const IIeee80211Mode* RateSelection::computeControlFrameMode(const Ptr<Ieee80211Frame>& frame)
{
    // TODO: BSSBasicRateSet
    return fastestMandatoryMode;
}

const IIeee80211Mode* RateSelection::computeMode(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    if (auto dataOrMgmtFrame = std::dynamic_pointer_cast<Ieee80211DataOrMgmtFrame>(frame))
        return computeDataOrMgmtFrameMode(dataOrMgmtFrame);
    else
        return computeControlFrameMode(frame);
}

void RateSelection::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details)
{
    Enter_Method("receiveModeSetChangeNotification");
    if (signalID == NF_MODESET_CHANGED) {
        modeSet = check_and_cast<Ieee80211ModeSet*>(obj);
        fastestMandatoryMode = modeSet->getFastestMandatoryMode();
    }
}

void RateSelection::frameTransmitted(Packet *packet, const Ptr<Ieee80211Frame>& frame)
{
    auto receiverAddr = frame->getReceiverAddress();
    lastTransmittedFrameMode[receiverAddr] = getMode(packet, frame);
}

void RateSelection::setFrameMode(Packet *packet, const Ptr<Ieee80211Frame>& frame, const IIeee80211Mode *mode)
{
    ASSERT(mode != nullptr);
    packet->ensureTag<Ieee80211ModeReq>()->setMode(mode);
}


} // namespace ieee80211
} // namespace inet
