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

#include "Ieee80211MacImmediateTx.h"
#include "Ieee80211NewMac.h"
#include "Ieee80211UpperMac.h"

namespace inet {

namespace ieee80211 {

Ieee80211MacImmediateTx::Ieee80211MacImmediateTx(Ieee80211NewMac *mac) : Ieee80211MacPlugin(mac)
{
    endImmediateIFS = new cMessage("Immediate IFS");
    immediateFrameDuration = new cMessage("Immediate Frame Duration");
}

Ieee80211MacImmediateTx::~Ieee80211MacImmediateTx()
{
}

void Ieee80211MacImmediateTx::transmitImmediateFrame(Ieee80211Frame* frame, simtime_t deferDuration, ITransmissionCompleteCallback *transmissionCompleteCallback)
{
    //TODO assert !ongoing
    scheduleAt(simTime() + deferDuration, endImmediateIFS);
    immediateFrame = frame;
    this->transmissionCompleteCallback = transmissionCompleteCallback;
}

void Ieee80211MacImmediateTx::transmissionStateChanged(IRadio::TransmissionState transmissionState)
{
    if (immediateFrameTransmission && transmissionState == IRadio::TRANSMISSION_STATE_IDLE)
        transmissionCompleteCallback->transmissionComplete(nullptr);
}

void Ieee80211MacImmediateTx::handleMessage(cMessage *msg)
{
    if (msg == endImmediateIFS) {
        immediateFrameTransmission = true;
        mac->sendFrame(immediateFrame);
    }
    else
        ASSERT(false);
}

}

} //namespace

