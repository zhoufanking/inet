//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#include "LineSegmentsMobilityBase.h"
#include "FWMath.h"


LineSegmentsMobilityBase::LineSegmentsMobilityBase()
{
    targetPosition = Coord::ZERO;
    lastTargetPosition = Coord::ZERO;
}

void LineSegmentsMobilityBase::initializePosition()
{
    MobilityBase::initializePosition();
    lastTargetPosition = lastPosition;
    if (!stationary) {
        setTargetPosition();
        EV_INFO << "current target position = " << targetPosition << ", next change = " << nextChange << endl;
        lastChangeSpeed = lastSpeed = (targetPosition - lastTargetPosition) / (nextChange - simTime()).dbl();
    }
    lastUpdate = lastChange = simTime();
    scheduleUpdate();
}

void LineSegmentsMobilityBase::move()
{
    simtime_t now = simTime();
    if (now > lastUpdate)
    {
        double delta = (now - lastChange) / (nextChange - lastChange);
        lastPosition = lastTargetPosition + (targetPosition - lastTargetPosition) * delta;
        lastSpeed = lastChangeSpeed;
        handleIfOutside();
    }

    if (now == nextChange) {
        lastChange = now;
        lastTargetPosition = lastPosition;
        EV_INFO << "reached current target position = " << lastPosition << endl;
        setTargetPosition();
        EV_INFO << "new target position = " << targetPosition << ", next change = " << nextChange << endl;
        lastChangeSpeed = lastSpeed = (targetPosition - lastTargetPosition) / (nextChange - simTime()).dbl();
    }
}
