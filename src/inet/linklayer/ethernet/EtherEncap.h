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

#ifndef __INET_ETHERENCAP_H
#define __INET_ETHERENCAP_H

#include "inet/common/INETDefs.h"

#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/linklayer/ethernet/Ethernet.h"

namespace inet {

// Forward declarations:
class EtherFrame;

/**
 * Performs Ethernet II encapsulation/decapsulation. More info in the NED file.
 */
class INET_API EtherEncap : public cSimpleModule, public ILifecycle
{
  public:
    struct DsapAndSocketId {
        int dsap;
        int socketId;
        DsapAndSocketId(int dsap, int socketId) : dsap(dsap), socketId(socketId) {}
        bool operator==(const DsapAndSocketId& o) const { return dsap == o.dsap && socketId == o.socketId; }
    };
  protected:
    int seqNum;
    std::vector<DsapAndSocketId> dsapToSocketIds;    // DSAP registration table
    bool useSNAP;    //TODO needed per packet setting // true: generate EtherFrameWithSNAP, false: generate EthernetIIFrame

    // lifecycle
    bool isUp;

    // statistics
    long totalFromHigherLayer;    // total number of packets received from higher layer
    long totalFromMAC;    // total number of frames received from MAC
    long totalPauseSent;    // total number of PAUSE frames sent
    long droppedUnknownDest;    // frames dropped because no such DSAP was registered here
    static simsignal_t encapPkSignal;
    static simsignal_t decapPkSignal;
    static simsignal_t pauseSentSignal;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void processPacketFromHigherLayer(cPacket *msg);
    virtual void processFrameFromMAC(EtherFrame *msg);
    virtual void handleRegisterSAP(cMessage *msg);
    virtual void handleDeregisterSAP(cMessage *msg);
    virtual void handleSendPause(cMessage *msg);

    // lifecycle
    virtual void start();
    virtual void stop();
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

    // utility function
    virtual void refreshDisplay() const override;
};

inline std::ostream& operator<<(std::ostream& o, const EtherEncap::DsapAndSocketId& i)
{
    o << "dsap=" << i.dsap << ", socket=" << i.socketId;
    return o;
}

} // namespace inet

#endif // ifndef __INET_ETHERENCAP_H

