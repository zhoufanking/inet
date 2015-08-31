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

#include "Ieee80211MacReception.h"
#include "Ieee80211NewMac.h"
#include "Ieee80211UpperMac.h"

namespace inet {

namespace ieee80211 {

Ieee80211MacReception::Ieee80211MacReception(Ieee80211NewMac* mac) : Ieee80211MacPlugin(mac)
{
    nav = new cMessage("NAV");
}


void Ieee80211MacReception::handleMessage(cMessage* msg)
{

}

void Ieee80211MacReception::handleLowerFrame(Ieee80211Frame* frame)
{
    EV << "received message from lower layer: " << frame << endl;
    if (!frame)
        opp_error("message from physical layer (%s)%s is not a subclass of Ieee80211Frame",
              frame->getClassName(), frame->getName());
    if (!getUpperMac()->isForUs(frame))
        setNav(frame->getDuration());
    Ieee80211TwoAddressFrame *twoAddressFrame = dynamic_cast<Ieee80211TwoAddressFrame *>(frame);
    ASSERT(!twoAddressFrame || twoAddressFrame->getTransmitterAddress() != mac->address);
    getUpperMac()->lowerFrameReceived(frame);

}

bool Ieee80211MacReception::isMediumFree() const
{
    return receptionState == IRadio::RECEPTION_STATE_IDLE && transmissionState != IRadio::TRANSMISSION_STATE_TRANSMITTING && !nav->isScheduled();
}

void Ieee80211MacReception::receptionStateChanged(IRadio::ReceptionState newReceptionState)
{
    receptionState = newReceptionState;
}

void Ieee80211MacReception::transmissionStateChanged(IRadio::TransmissionState newTransmissionState)
{
    transmissionState = newTransmissionState;
}

void Ieee80211MacReception::setNav(simtime_t navInterval)
{
    ASSERT(navInterval > 0);
    if (nav->isScheduled())
    {
        simtime_t oldNav = nav->getArrivalTime() - simTime();
        if (oldNav > navInterval)
            return;
        cancelEvent(nav);
    }
    EV_INFO << "Setting NAV to " << navInterval << std::endl;
    scheduleAt(simTime() + navInterval, nav);
}

}

} /* namespace inet */

