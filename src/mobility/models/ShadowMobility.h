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


#ifndef __INET_SHADOWMOBILITY_H
#define __INET_SHADOWMOBILITY_H

#include "INETDefs.h"

#include "MovingMobilityBase.h"


/**
 * @brief Circle movement model. See NED file for more info.
 *
 * @ingroup mobility
 * @author Andras Varga
 */
class INET_API ShadowMobility : public MobilityBase, cListener
{
  protected:
    static simsignal_t mobilityStateChangedSignal;
    IMobility *masterMobility;

  protected:
    /** @brief Initializes mobility model parameters.*/
    virtual void initialize(int stage);

    /** @brief Initializes the position according to the mobility model. */
    virtual void initializePosition();

    virtual void handleSelfMessage(cMessage *msg) { throw cRuntimeError("model error: ShadowMobility can't receive self messages"); }

   /**
     * @brief Called by the signaling mechanism to inform of changes.
     *
     * ShadowMobility is subscribed to position changes.
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);

    /** @brief Returns the current position at the current simulation time. */
    virtual Coord getCurrentPosition();

    /** @brief Returns the current speed at the current simulation time. */
    virtual Coord getCurrentSpeed();

  public:
    ShadowMobility();
};

#endif
