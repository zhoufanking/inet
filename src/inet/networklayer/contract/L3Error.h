#ifndef __INET_L3ERROR_H
#define __INET_L3ERROR_H


#include "inet/networklayer/contract/L3Error_m.h"

#include "inet/common/ProtocolGroup.h"
#include "inet/common/IPacketControlInfo.h"

namespace inet {

class INET_API L3ErrorControlInfo : public L3ErrorControlInfo_Base, public IPacketControlInfo
{
  protected:
    INetworkProtocolControlInfo *networkProtocolControlInfo = nullptr;
  private:
    void copy(const L3ErrorControlInfo& other);
    void clean();
  public:
    L3ErrorControlInfo() : L3ErrorControlInfo_Base() {}
    L3ErrorControlInfo(const L3ErrorControlInfo& other) : L3ErrorControlInfo_Base(other) { copy(other); }
    ~L3ErrorControlInfo() { clean(); }
    L3ErrorControlInfo& operator=(const L3ErrorControlInfo& other);
    virtual L3ErrorControlInfo *dup() const override { return new L3ErrorControlInfo(*this); }

    virtual L3Address getSourceAddress() const override { return _getSourceAddress(); }
    virtual L3Address getDestinationAddress() const override { return _getDestinationAddress(); }
    virtual int getPacketProtocolId() const override { return ProtocolGroup::ipprotocol.getProtocol(getTransportProtocol())->getId(); }

    virtual INetworkProtocolControlInfoPtr& getNetworkProtocolControlInfo() override { return networkProtocolControlInfo; }
    virtual INetworkProtocolControlInfo *removeNetworkProtocolControlInfo();
    virtual const INetworkProtocolControlInfoPtr& getNetworkProtocolControlInfo() const override { return const_cast<L3ErrorControlInfo *>(this)->getNetworkProtocolControlInfo(); }
    virtual void setNetworkProtocolControlInfo(const INetworkProtocolControlInfoPtr& networkProtocolControlInfo) override;
};

} // namespace inet

#endif // ifndef __INET_L3ERROR_H

