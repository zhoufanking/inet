//
// Copyright (C) OpenSim Ltd.
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
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#ifndef __INET_IEEE80211STA_H
#define __INET_IEEE80211STA_H

#include "inet/common/INETDefs.h"

namespace inet {

namespace ieee80211 {

class INET_API Ieee80211Sta
{
  public:
    enum BssMemberStatus {
        NOT_AUTHENTICATED,
        AUTHENTICATED,
        ASSOCIATED
    };

    bool isAp;
    bool isBssMember;
    BssMemberStatus bssMemberStatus;
};

} // namespace ieee80211

} // namespace inet

#endif // __INET_IEEE80211STA_H

