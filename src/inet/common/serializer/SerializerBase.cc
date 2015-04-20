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


//TODO split it to more files

#include "inet/common/serializer/SerializerBase.h"

namespace inet {

namespace serializer {

SerializerRegistrationList serializers("default"); ///< List of packet serializers (SerializerBase)

EXECUTE_ON_SHUTDOWN(
        serializers.clear();
        );

DefaultSerializer SerializerRegistrationList::defaultSerializer;
ByteArraySerializer SerializerRegistrationList::byteArraySerializer;

void SerializerBase::serializePacket(const cPacket *pkt, Buffer &b, Context& c)
{
    unsigned int startPos = b.getPos();
    serialize(pkt, b, c);
    if (!b.hasError() && (b.getPos() - startPos != pkt->getByteLength())) {
        b.seek(startPos);
        serialize(pkt, b, c);
        throw cRuntimeError("%s serializer error: packet %s (%s) length is %d but serialized length is %d", getClassName(), pkt->getName(), pkt->getClassName(), pkt->getByteLength(), b.getPos() - startPos);
    }
}

cPacket *SerializerBase::deserializePacket(const Buffer &b, Context& context)
{
    if (b.getRemainingSize() == 0)
        return nullptr;

    unsigned int startPos = b.getPos();
    cPacket *pkt = deserialize(b, context);
    if (pkt == nullptr) {
        b.seek(startPos);
        pkt = serializers.byteArraySerializer.deserialize(b, context);
    }
    ASSERT(pkt);
    if (!pkt->hasBitError() && !b.hasError() && (b.getPos() - startPos != pkt->getByteLength())) {
        const char *encclass = pkt->getEncapsulatedPacket() ? pkt->getEncapsulatedPacket()->getClassName() : "<nullptr>";
        throw cRuntimeError("%s deserializer error: packet %s (%s) length is %d but deserialized length is %d (encapsulated packet is %s)", getClassName(), pkt->getName(), pkt->getClassName(), pkt->getByteLength(), b.getPos() - startPos, encclass);
    }
    if (b.hasError())
        pkt->setBitError(true);
    return pkt;
}

SerializerBase & SerializerBase::lookupSerializer(const cPacket *pkt, Context& context, ProtocolGroup group, int id)
{
    const RawPacket *bam = dynamic_cast<const RawPacket *>(pkt);
    if (bam != nullptr)
        return serializers.byteArraySerializer;
    SerializerBase *serializer = serializers.lookup(group, id);
    if (serializer != nullptr)
        return *serializer;
    serializer = serializers.lookup(pkt->getClassName());
    if (serializer != nullptr)
        return *serializer;
    if (context.throwOnSerializerNotFound)
        throw cRuntimeError("Serializer not found for '%s' (%i, %i)", pkt->getClassName(), group, id);
    return serializers.defaultSerializer;
}

SerializerBase & SerializerBase::lookupDeserializer(Context& context, ProtocolGroup group, int id)
{
    SerializerBase *serializer = serializers.lookup(group, id);
    if (serializer != nullptr)
        return *serializer;
    else
        return serializers.byteArraySerializer;
}

void SerializerBase::lookupAndSerialize(const cPacket *pkt, Buffer &b, Context& context, ProtocolGroup group, int id, unsigned int maxLength)
{
    ASSERT(pkt);
    Buffer subBuffer(b, maxLength);
    SerializerBase & serializer = lookupSerializer(pkt, context, group, id);
    serializer.serializePacket(pkt, subBuffer, context);
    b.accessNBytes(subBuffer.getPos());
    if (subBuffer.hasError())
        b.setError();
}

void SerializerBase::lookupAndSerializeAndCheck(const cPacket *pkt, Buffer &b, Context& c, ProtocolGroup group, int id, unsigned int maxLength)
{
    ASSERT(pkt);
    Buffer subBuffer(b, maxLength);
    SerializerBase & serializer = lookupSerializer(pkt, c, group, id);
    serializer.serializePacket(pkt, subBuffer, c);
    b.accessNBytes(subBuffer.getPos());
    if (subBuffer.hasError())
        b.setError();
#if 1   //DEBUG only
    Buffer b1(subBuffer._getBuf(), subBuffer._getBufSize());
    Context c1(c);
    cPacket *deserialized = serializer.deserialize(b1, c1);
    if (deserialized) {
        std::ostringstream to, td;
        for (const cPacket *p = pkt; p; p = p->getEncapsulatedPacket()) {
            to << p->getClassName() << "(" << p->getByteLength() << ")/";
        }
        for (const cPacket *p = deserialized; p; p = p->getEncapsulatedPacket()) {
            td << p->getClassName() << "(" << p->getByteLength() << ")/";
        }
        int64 length = subBuffer.hasError() ? subBuffer.getPos() : pkt->getByteLength();
        const cPacket *po = pkt, *pd = deserialized;
        int64 olen = po->getByteLength() + 1;
        while (po && pd) {
            if (po->getByteLength() > olen) {
                break;          // encapsulated chunk, deserialized length should be differ, it's OK
            }
            if (pd != deserialized && dynamic_cast<const RawPacket *>(pd)) {
                break;          // deserialize to RawPacket eat all padding bytes, parent packet length was OK, it's OK
            }
            if ((po->getByteLength() != pd->getByteLength())) {
                Buffer bx(subBuffer._getBuf(), subBuffer._getBufSize());
                Context cx(c);
                cPacket *deserialized2 = serializer.deserialize(bx, cx);
                for (int64_t i = 0; i < length && i < subBuffer._getBufSize(); i++) {
                    if (i % 16 == 0) std::cout << std::endl;
                    char buf[20]; sprintf(buf, " %02X", (unsigned int)(subBuffer._getBuf()[i])); std::cout << buf;
                }
                std::cout << std::endl;
                throw cRuntimeError("%s serializer error: packet %s (%s) and serialized-deserialized (%s) length are differ at %s:%s", serializer.getClassName(), pkt->getName(), to.str().c_str(), td.str().c_str(), po->getClassName(), pd->getClassName());
            }
            if (pd == deserialized && typeid(*pd) == typeid(cPacket))
                break; // using DefaultSerializer in top level, OK
            if (typeid(*po) != typeid(*pd)) {
                throw cRuntimeError("%s serializer error: packet %s (%s) and serialized-deserialized (%s) types are differ at %s:%s", serializer.getClassName(), pkt->getName(), to.str().c_str(), td.str().c_str(), po->getClassName(), pd->getClassName());
            }
            olen = po->getByteLength();
            po = po->getEncapsulatedPacket();
            pd = pd->getEncapsulatedPacket();
        }
        unsigned char *buffer2 = new unsigned char[length];
        Buffer b2(buffer2, length);
        Context c2(c);
        serializer.serialize(deserialized, b2, c2);
        for (int64_t n = 0; n < length; n++) {
            if (subBuffer._getBuf()[n] != buffer2[n]) {
                Buffer bx(buffer2, length);
                Context cx(c);
                serializer.serialize(deserialized, bx, cx);
                for (int64_t i = 0; i < length && i < subBuffer._getBufSize(); i++) {
                    if (i % 16 == 0)
                        std::cout << std::endl;
                    char buf[20];
                    sprintf(buf, " %02X:%02X%c", (unsigned int)(subBuffer._getBuf()[i]), (unsigned int)(buffer2[i]), ((subBuffer._getBuf()[i] != buffer2[i]) ? '*' : ' '));
                    std::cout << buf;
                }
                std::cout << std::endl;
                throw cRuntimeError("%s serializer error: packet %s (%s) length is %d, serialized and serialized-deserialized-(%s)-serialized contents are differ at %d", serializer.getClassName(), pkt->getName(), to.str().c_str(), pkt->getByteLength(), td.str().c_str(), n);
            }
        }
        delete[] buffer2;
        delete deserialized;
    }
#endif
}

cPacket *SerializerBase::lookupAndDeserialize(const Buffer &b, Context& context, ProtocolGroup group, int id, unsigned int maxLength)
{
    cPacket *encapPacket = nullptr;
    SerializerBase& serializer = lookupDeserializer(context, group, id);
    Buffer subBuffer(b, maxLength);
    encapPacket = serializer.deserializePacket(subBuffer, context);
    b.accessNBytes(subBuffer.getPos());
    return encapPacket;
}

//

void DefaultSerializer::serialize(const cPacket *pkt, Buffer &b, Context& context)
{
    b.fillNBytes(pkt->getByteLength(), '?');
    context.errorOccured = true;
}

cPacket *DefaultSerializer::deserialize(const Buffer &b, Context& context)
{
    unsigned int byteLength = b.getRemainingSize();
    if (byteLength) {
        cPacket *pkt = new cPacket();
        pkt->setByteLength(byteLength);
        b.accessNBytes(byteLength);
        context.errorOccured = true;
        return pkt;
    }
    else
        return nullptr;
}

//

void ByteArraySerializer::serialize(const cPacket *pkt, Buffer &b, Context& context)
{
    const RawPacket *bam = check_and_cast<const RawPacket *>(pkt);
    unsigned int length = bam->getByteLength();
    unsigned int wl = std::min(length, b.getRemainingSize());
    wl = bam->copyDataToBuffer(b.accessNBytes(0), wl);
    b.accessNBytes(wl);
    if (length > wl)
        b.fillNBytes(length - wl, '?');
    if (pkt->getEncapsulatedPacket())
        throw cRuntimeError("Serializer: encapsulated packet in ByteArrayPacket is not allowed");
}

cPacket *ByteArraySerializer::deserialize(const Buffer &b, Context& context)
{
    RawPacket *bam = nullptr;
    unsigned int bytes = b.getRemainingSize();
    if (bytes) {
        bam = new RawPacket("parsed-bytes");
        bam->setDataFromBuffer(b.accessNBytes(bytes), bytes);
        bam->setByteLength(bytes);
    }
    return bam;
}

//

SerializerRegistrationList::~SerializerRegistrationList()
{
    if (!stringToSerializerMap.empty())
        throw cRuntimeError("SerializerRegistrationList not empty, should call the SerializerRegistrationList::clear() function");
}

void SerializerRegistrationList::clear()
{
    for (auto elem : stringToSerializerMap) {
        dropAndDelete(elem.second);
    }
    stringToSerializerMap.clear();
    keyToSerializerMap.clear();
}

void SerializerRegistrationList::add(const char *name, int protocolGroup, int protocolId, SerializerBase *obj)
{
    Key key(protocolGroup, protocolId);

    take(obj);
    if (protocolGroup != UNKNOWN)
        keyToSerializerMap.insert(std::pair<Key,SerializerBase*>(key, obj));
    if (!name)
        throw cRuntimeError("missing 'name' of registered serializer");
    stringToSerializerMap.insert(std::pair<std::string,SerializerBase*>(name, obj));
}

SerializerBase *SerializerRegistrationList::lookup(int protocolGroup, int protocolId) const
{
    auto it = keyToSerializerMap.find(Key(protocolGroup, protocolId));
    return it==keyToSerializerMap.end() ? NULL : it->second;
    return nullptr;
}

SerializerBase *SerializerRegistrationList::lookup(const char *name) const
{
    auto it = stringToSerializerMap.find(name);
    return it==stringToSerializerMap.end() ? NULL : it->second;
    return nullptr;
}

//

} // namespace serializer

} // namespace inet

