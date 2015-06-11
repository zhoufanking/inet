//
// Copyright (C) 2015 Irene Ruengeler
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

#ifndef __INET_TUNNELAPP_H
#define __INET_TUNNELAPP_H

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

namespace inet {

class INET_API TunnelApp : public ApplicationBase
{
protected:
    const char *tunInterface = nullptr;
    const char *destAddress = nullptr;
    int destPort = -1;
    int localPort = -1;

    UDPSocket serverSocket;
    UDPSocket clientSocket;

public:
    TunnelApp();
    virtual ~TunnelApp();

protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
};

} // namespace inet

#endif // ifndef __INET_TUNNELAPP_H

