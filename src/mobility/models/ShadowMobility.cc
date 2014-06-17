//
// Copyright (C) 2005 Andras Varga
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


#include "ShadowMobility.h"
#include "FWMath.h"


Define_Module(ShadowMobility);

simsignal_t ShadowMobility::mobilityStateChangedSignal = SIMSIGNAL_NULL;

ShadowMobility::ShadowMobility()
{
    masterMobility = NULL;
}

void ShadowMobility::initialize(int stage)
{
    MobilityBase::initialize(stage);
    EV << "initializing ShadowMobility stage " << stage << endl;
    if (stage == 0)
    {
        mobilityStateChangedSignal = registerSignal("mobilityStateChanged");
        cModule *masterMobilityModule = getModuleByPath(par("masterMobility").stringValue());
        masterMobility = check_and_cast<IMobility *>(masterMobilityModule);
        masterMobilityModule->subscribe(mobilityStateChangedSignal, this);
    }
}

void ShadowMobility::initializePosition()
{
    lastPosition = masterMobility->getCurrentPosition();
}

void ShadowMobility::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    if (signalID == mobilityStateChangedSignal)
    {
        IMobility *mobility = check_and_cast<IMobility*>(obj);
        if (mobility == masterMobility)
        {
            lastPosition = mobility->getCurrentPosition();
            emit(mobilityStateChangedSignal, this);
        }
    }
}

Coord ShadowMobility::getCurrentPosition()
{
    return masterMobility ? masterMobility->getCurrentPosition() : lastPosition;
}

Coord ShadowMobility::getCurrentSpeed()
{
    return masterMobility ? masterMobility->getCurrentSpeed() : Coord::ZERO;
}

