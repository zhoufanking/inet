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

#ifndef __INET_IPV4DATANOTIFICATIONDATA_H
#define __INET_IPV4DATANOTIFICATIONDATA_H

#include "inet/common/INETDefs.h"

#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"

namespace inet {

/**
 * TODO
 */
class INET_API IPv4DataNotificationData : public cObject
{
  public:
    IPv4Datagram *datagram = nullptr;
    const InterfaceEntry *fromIE = nullptr;

  public:
    IPv4DataNotificationData(IPv4Datagram *datagram, const InterfaceEntry *fromIE) : datagram(datagram), fromIE(fromIE) {}
};

} // namespace inet

#endif // ifndef __INET_IPV4DATANOTIFICATIONDATA_H

