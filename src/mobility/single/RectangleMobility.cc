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


#include "RectangleMobility.h"
#include "FWMath.h"


Define_Module(RectangleMobility);


RectangleMobility::RectangleMobility()
{
    speed = 0;
    d = 0;
    corner1 = corner2 = corner3 = corner4 = 0;
}

void RectangleMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);

    EV_TRACE << "initializing RectangleMobility stage " << stage << endl;
    if (stage == INITSTAGE_LOCAL)
    {
        startTime = simTime();
        speed = par("speed");
        stationary = (speed == 0);

        // calculate helper variables
        double dx = constraintAreaMax.x - constraintAreaMin.x;
        double dy = constraintAreaMax.y - constraintAreaMin.y;
        corner1 = dx;
        corner2 = corner1 + dy;
        corner3 = corner2 + dx;
        corner4 = corner3 + dy;

        // determine start position
        double startPos = par("startPos");
        startPos = fmod(startPos, 4);
        if (startPos < 1)
            d = startPos * dx; // top side
        else if (startPos < 2)
            d = corner1 + (startPos - 1) * dy; // right side
        else if (startPos < 3)
            d = corner2 + (startPos - 2) * dx; // bottom side
        else
            d = corner3 + (startPos - 3) * dy; // left side
        WATCH(d);
    }
}

void RectangleMobility::setInitialPosition()
{
    move();
}

void RectangleMobility::move()
{
    double elapsedTime = (simTime() - startTime).dbl();
    double recPos = d + speed * elapsedTime;

    while (recPos < 0)
        recPos += corner4;

    while (d >= corner4)
        recPos -= corner4;

    if (recPos < corner1)
    {
        // top side
        lastPosition.x = constraintAreaMin.x + recPos;
        lastPosition.y = constraintAreaMin.y;
    }
    else if (recPos < corner2)
    {
        // right side
        lastPosition.x = constraintAreaMax.x;
        lastPosition.y = constraintAreaMin.y + recPos - corner1;
    }
    else if (recPos < corner3)
    {
        // bottom side
        lastPosition.x = constraintAreaMax.x - recPos + corner2;
        lastPosition.y = constraintAreaMax.y;
    }
    else
    {
        // left side
        lastPosition.x = constraintAreaMin.x;
        lastPosition.y = constraintAreaMax.y - recPos + corner3;
    }
}
