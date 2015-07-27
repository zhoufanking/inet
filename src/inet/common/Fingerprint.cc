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

#include "inet/common/Fingerprint.h"
#include "inet/common/serializer/SerializerBase.h"

namespace inet {

Register_Class(Fingerprint);

using namespace inet::serializer;

bool Fingerprint::addEventCategory(cEvent *event, FingprintCategory category)
{
    if (category == MESSAGE_DATA) {
        cMessage *message = event->isMessage() ? static_cast<cMessage *>(event) : nullptr;
        cPacket *packet = message->isPacket() ? static_cast<cPacket *>(message) : nullptr;
        if (packet != nullptr) {
            int length = packet->getByteLength();
            char *buffer = new char[length];
            Buffer b(buffer, length);
            Context c;
            SerializerBase::lookupAndSerialize(packet, b, c, UNKNOWN, -1);
            hasher->add(buffer, length);
            delete [] buffer;
        }
        return true;
    }
    else
        return false;
}

} // namespace inet

