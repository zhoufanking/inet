//
// (C) 2005 Vojtech Janota
// (C) 2010 Zoltan Bojthe
//
// This library is free software, you can redistribute it
// and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

#ifndef __INET_FINGERPRINT_H
#define __INET_FINGERPRINT_H

#include "inet/common/INETDefs.h"

namespace inet {

class INET_API Fingerprint : public cSingleFingerprint
{
  public:
    virtual cFingerprint *dup() const override { return new Fingerprint(); }
    virtual bool addEventCategory(cEvent *event, FingprintCategory category) override;
};

} // namespace inet

#endif // ifndef __INET_FINGERPRINT_H

