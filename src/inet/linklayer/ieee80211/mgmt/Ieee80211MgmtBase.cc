//
// Copyright (C) 2006 Andras Varga
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

#include "inet/common/ModuleAccess.h"
#include "inet/common/INETUtils.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h"

namespace inet {

namespace ieee80211 {

void Ieee80211MgmtBase::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        mib = getModuleFromPar<Ieee80211Mib>(par("mibModule"), this);
        numMgmtFramesReceived = 0;
        numMgmtFramesDropped = 0;
        WATCH(numMgmtFramesReceived);
        WATCH(numMgmtFramesDropped);
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
    }
    else if (stage == INITSTAGE_LINK_LAYER_2) {
        // obtain our address from MAC
        cModule *mac = getModuleFromPar<cModule>(par("macModule"), this);
        myAddress.setAddress(mac->par("address").stringValue());
        interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        myIface = interfaceTable->getInterfaceByName(utils::stripnonalnum(findModuleUnderContainingNode(this)->getFullName()).c_str());
    }
}

void Ieee80211MgmtBase::handleMessage(cMessage *msg)
{
    if (!isOperational)
        throw cRuntimeError("Message '%s' received when module is OFF", msg->getName());

    if (msg->isSelfMessage()) {
        // process timers
        EV << "Timer expired: " << msg << "\n";
        handleTimer(msg);
    }
    else if (msg->arrivedOn("macIn")) {
        // process incoming frame
        EV << "Frame arrived from MAC: " << msg << "\n";
        auto packet = check_and_cast<Packet *>(msg);
        const Ptr<Ieee80211DataOrMgmtFrame>& frame = packet->peekHeader<Ieee80211DataOrMgmtFrame>();
        processFrame(packet, frame);
    }
    else if (msg->arrivedOn("agentIn")) {
        // process command from agent
        EV << "Command arrived from agent: " << msg << "\n";
        int msgkind = msg->getKind();
        cObject *ctrl = msg->removeControlInfo();
        delete msg;

        handleCommand(msgkind, ctrl);
    }
    else
        throw cRuntimeError("Unknown message");
}

void Ieee80211MgmtBase::sendDown(Packet *frame)
{
    ASSERT(isOperational);
    send(frame, "macOut");
}

void Ieee80211MgmtBase::dropManagementFrame(Packet *frame)
{
    EV << "ignoring management frame: " << (cMessage *)frame << "\n";
    delete frame;
    numMgmtFramesDropped++;
}

void Ieee80211MgmtBase::sendUp(cMessage *msg)
{
    ASSERT(isOperational);
    msg->ensureTag<InterfaceInd>()->setInterfaceId(myIface->getInterfaceId());
    send(msg, "upperLayerOut");
}

void Ieee80211MgmtBase::processFrame(Packet *packet, const Ptr<Ieee80211DataOrMgmtFrame>& frame)
{
    switch (frame->getType()) {
        case ST_AUTHENTICATION:
            numMgmtFramesReceived++;
            handleAuthenticationFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_DEAUTHENTICATION:
            numMgmtFramesReceived++;
            handleDeauthenticationFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_ASSOCIATIONREQUEST:
            numMgmtFramesReceived++;
            handleAssociationRequestFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_ASSOCIATIONRESPONSE:
            numMgmtFramesReceived++;
            handleAssociationResponseFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_REASSOCIATIONREQUEST:
            numMgmtFramesReceived++;
            handleReassociationRequestFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_REASSOCIATIONRESPONSE:
            numMgmtFramesReceived++;
            handleReassociationResponseFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_DISASSOCIATION:
            numMgmtFramesReceived++;
            handleDisassociationFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_BEACON:
            numMgmtFramesReceived++;
            handleBeaconFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_PROBEREQUEST:
            numMgmtFramesReceived++;
            handleProbeRequestFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        case ST_PROBERESPONSE:
            numMgmtFramesReceived++;
            handleProbeResponseFrame(packet, std::dynamic_pointer_cast<Ieee80211ManagementHeader>(frame));
            break;

        default:
            throw cRuntimeError("Unexpected frame type (%s)%s", frame->getClassName(), frame->getName());
    }
}

bool Ieee80211MgmtBase::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_PHYSICAL_LAYER)
            start();
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_PHYSICAL_LAYER)
            stop();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH) // crash is immediate
            stop();
    }
    else
        throw cRuntimeError("Unsupported operation '%s'", operation->getClassName());
    return true;
}

void Ieee80211MgmtBase::start()
{
    isOperational = true;
}

void Ieee80211MgmtBase::stop()
{
    isOperational = false;
}

} // namespace ieee80211

} // namespace inet

