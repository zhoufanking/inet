//
// Author: Emin Ilker Cetinbas (niw3_at_yahoo_d0t_com)
// Copyright (C) 2005 Emin Ilker Cetinbas
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


#include "LinearMobility.h"
#include "FWMath.h"


Define_Module(LinearMobility);


LinearMobility::LinearMobility()
{
    speed = 0;
    angle = 0;
    acceleration = 0;
}

void LinearMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);

    EV_TRACE << "initializing LinearMobility stage " << stage << endl;
    if (stage == INITSTAGE_LOCAL)
    {
        startTime = simTime();
        speed = par("speed");
        angle = fmod((double)par("angle"), 360);
        acceleration = par("acceleration");
        stationary = (speed == 0) && (acceleration == 0.0);
    }
}

void LinearMobility::initializePosition()
{
    MobilityBase::initializePosition();
    startPosition = lastPosition;
}

void LinearMobility::move()
{
    double rad = PI * angle / 180;
    Coord direction(cos(rad), sin(rad));

    double elapsedTime = (simTime() - startTime).dbl();

    // when we reached zero speed
    if (acceleration < 0 && elapsedTime >= (-speed)/acceleration)
    {
        stationary = true;
        lastSpeed = Coord::ZERO;
        lastPosition = startPosition + (direction * (speed / 2 * (-speed)/acceleration));
    }
    else
    {
        double currentSpeed = speed + acceleration * elapsedTime;
        lastSpeed = direction * currentSpeed;
        lastPosition = startPosition + (direction * ((speed + currentSpeed) / 2 * elapsedTime));
    }

    Coord dummy;
    double dummyAngle;
    reflectIfOutside(dummy, lastSpeed, dummyAngle);


}
