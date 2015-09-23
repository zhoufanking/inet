
#include "inet/networklayer/contract/L3Error.h"

namespace inet {


void L3ErrorControlInfo::clean()
{
    if (networkProtocolControlInfo) {
        cObject *p = check_and_cast<cObject *>(networkProtocolControlInfo);
        if (p->isOwnedObject())
            dropAndDelete(static_cast<cOwnedObject *>(p));
        else
            delete p;
        networkProtocolControlInfo = nullptr;
    }
}

void L3ErrorControlInfo::copy(const L3ErrorControlInfo& other)
{
    if (other.networkProtocolControlInfo == nullptr)
        networkProtocolControlInfo = nullptr;
    else {
        cObject *p = check_and_cast<cObject *>(other.networkProtocolControlInfo)->dup();
        networkProtocolControlInfo = check_and_cast<INetworkProtocolControlInfoPtr>(p);
        if (p->isOwnedObject())
            take(static_cast<cOwnedObject *>(p));
    }
}

L3ErrorControlInfo& L3ErrorControlInfo::operator=(const L3ErrorControlInfo& other)
{
    if (this==&other)
        return *this;
    clean();
    L3ErrorControlInfo_Base::operator=(other);
    copy(other);
    return *this;
}

void L3ErrorControlInfo::setNetworkProtocolControlInfo(const INetworkProtocolControlInfoPtr& iPtr)
{
    if (!iPtr)
        throw cRuntimeError(this, "setNetworkProtocolControlInfo(): pointer is nullptr");
    if (networkProtocolControlInfo)
        throw cRuntimeError(this, "setNetworkProtocolControlInfo(): packet already has networkProtocolControlInfo attached");
    cObject *p = check_and_cast<cObject *>(iPtr);
    if (p->isOwnedObject())
        take(static_cast<cOwnedObject *>(p));
    networkProtocolControlInfo = iPtr;
}

INetworkProtocolControlInfo *L3ErrorControlInfo::removeNetworkProtocolControlInfo()
{
    INetworkProtocolControlInfo *ret = networkProtocolControlInfo;
    networkProtocolControlInfo = nullptr;
    cObject *p = check_and_cast<cObject *>(ret);
    if (p && p->isOwnedObject())
        drop(static_cast<cOwnedObject *>(p));
    return ret;
}

} // namespace inet

