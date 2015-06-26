#ifndef __INET_ICMPERRORCONTROLINFO_H
#define __INET_ICMPERRORCONTROLINFO_H


#include "inet/networklayer/contract/IcmpErrorControlInfo_m.h"

#include "inet/common/ProtocolGroup.h"
#include "inet/common/IPacketControlInfo.h"

namespace inet {

class INET_API IcmpErrorControlInfo : public IcmpErrorControlInfo_Base, public IPacketControlInfo
{
  public:
    IcmpErrorControlInfo() : IcmpErrorControlInfo_Base() {}
    IcmpErrorControlInfo(const IcmpErrorControlInfo& other) : IcmpErrorControlInfo_Base(other) {}
    IcmpErrorControlInfo& operator=(const IcmpErrorControlInfo& other) {if (this==&other) return *this; IcmpErrorControlInfo_Base::operator=(other); return *this;}
    virtual IcmpErrorControlInfo *dup() const override {return new IcmpErrorControlInfo(*this);}

    virtual L3Address getSourceAddress() const override { return _getSourceAddress(); }
    virtual L3Address getDestinationAddress() const override { return _getDestinationAddress(); }
    virtual int getPacketProtocolId() const override { return ProtocolGroup::ipprotocol.getProtocol(getTransportProtocol())->getId(); }
};

} // namespace inet

#endif // ifndef __INET_ICMPERRORCONTROLINFO_H

