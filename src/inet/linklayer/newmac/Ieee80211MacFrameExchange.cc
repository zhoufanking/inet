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

#include "Ieee80211MacFrameExchange.h"

namespace inet {

void Ieee80211FrameExchange::reportSuccess()
{
    finishedCallback->frameExchangeFinished(this, true);
}

void Ieee80211FrameExchange::reportFailure()
{
    finishedCallback->frameExchangeFinished(this, false);
}

//--------

void Ieee80211SendDataWithAckFrameExchange::handleWithFSM(EventType event, cMessage *frameOrTimer)
{
    Ieee80211Frame *receivedFrame = event == EVENT_FRAMEARRIVED ? check_and_cast<Ieee80211Frame*>(frameOrTimer) : nullptr;

    FSMA_Switch(fsm)
    {
        FSMA_State(INIT)
        {
            FSMA_Enter();
            FSMA_Event_Transition(Start,
                                  event == EVENT_START,
                                  TRANSMITDATA,
                                  transmitDataFrame();
            );
        }
        FSMA_State(TRANSMITDATA)
        {
            FSMA_Enter();
            FSMA_Event_Transition(Wait-ACK,
                                  event == EVENT_TXFINISHED,
                                  WAITACK,
                                  scheduleAckTimeout();
            );
            FSMA_Event_Transition(Frame-arrived,
                                  event == EVENT_FRAMEARRIVED,
                                  TRANSMITDATA,
                                  processFrame(receivedFrame);
            );
        }
        FSMA_State(WAITACK)
        {
            FSMA_Enter();
            FSMA_Event_Transition(Ack-arrived,
                                  event == EVENT_FRAMEARRIVED && isAck(receivedFrame),
                                  SUCCESS,
                                  delete receivedFrame;);
            );
            FSMA_Event_Transition(Frame-arrived,
                                  event == EVENT_FRAMEARRIVED && !isAck(receivedFrame),
                                  FAILURE,
                                  processFrame(receivedFrame);
            );
            FSMA_Event_Transition(Ack-timeout-retry,
                                  event == EVENT_TIMEOUT && retryCount < maxRetryCount,
                                  TRANSMITDATA,
                                  retryDataFrame();
            );
            FSMA_Event_Transition(Ack-timeout-giveup,
                                  event == EVENT_TIMEOUT && retryCount == maxRetryCount,
                                  FAILURE,
                                  ;
            );
        }
        FSMA_State(SUCCESS)
        {
            FSMA_Enter(reportSuccess()));
        }
        FSMA_State(FAILURE)
        {
            FSMA_Enter(reportFailure()));
        }
    }
}

void Ieee80211SendDataWithAckFrameExchange::transmitDataFrame()
{
    cw = cwMin;
    retryCount = 0;
    mac->transmitFrame(frame, ifs, cw);
}

void Ieee80211SendDataWithAckFrameExchange::retryDataFrame()
{
    if (cw < cwMax)
        cw = ((cw+1)>>1)-1;
    retryCount++;
    frame->setRetry(true);
    mac->transmitFrame(frame, ifs, cw);
}

void Ieee80211SendDataWithAckFrameExchange::scheduleAckTimeout()
{
    if (ackTimer == nullptr)
        ackTimer = new cMessage("timeout");
    simtime_t t = simTime() + something; //TODO
    scheduleAt(t, ackTimer);
}

void Ieee80211SendDataWithAckFrameExchange::processFrame(Ieee80211Frame *receivedFrame)
{
    //TODO some totally unrelated frame arrived; process in the normal way
}


} /* namespace inet */

#endif

