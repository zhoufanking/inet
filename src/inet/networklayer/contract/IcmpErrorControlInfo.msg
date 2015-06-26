//
// Copyright (C) 2015 OpenSim Ltd.
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
// @author Zoltan Bojthe
//

cplusplus {{
#include "inet/common/IProtocolControlInfo.h"
#include "inet/common/ISocketControlInfo.h"
#include "inet/common/Protocol.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/IPProtocolId_m.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"

namespace inet {
    typedef INetworkProtocolControlInfo *INetworkProtocolControlInfoPtr;
}

}}

namespace inet;

class noncobject L3Address;
class noncobject INetworkProtocolControlInfoPtr;

enum IcmpErrorControlInfoErrorCodes {
    ICMPERROR_DEST_UNREACHABLE = 1000;
    ICMPERROR_HOST_UNREACHABLE = 1001;
    ICMPERROR_PROTOCOL_UNREACHABLE = 1002;
    ICMPERROR_PORT_UNREACHABLE = 1003;
    ICMPERROR_TIME_EXCEEDED = 1004;
    ICMPERROR_FRAGMENTATION_NEEDED = 1005;
    ICMPERROR_PARAMETER_PROBLEM = 1006;
};

class IcmpErrorControlInfo
{
    @customize(true);
    @implements(INetworkProtocolControlInfo, IProtocolControlInfo, ISocketControlInfo);
    int controlInfoProtocolId = Protocol::icmpv4.getId();
    short transportProtocol = -1;
    int interfaceId = -1;
    int socketId = -1;
    short hopLimit = -1;
    L3Address sourceAddress @getter(_getSourceAddress);
    L3Address destinationAddress @getter(_getDestinationAddress);
    int errorCode @enum(IcmpErrorControlInfoErrorCodes) = -1;
    INetworkProtocolControlInfoPtr networkProtocolControlInfo;  // owned by IcmpErrorControlInfo
}

