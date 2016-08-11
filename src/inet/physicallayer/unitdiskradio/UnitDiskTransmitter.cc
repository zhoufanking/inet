//
// Copyright (C) 2013 OpenSim Ltd.
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

#include "inet/physicallayer/unitdiskradio/UnitDiskTransmission.h"
#include "inet/physicallayer/unitdiskradio/UnitDiskTransmitter.h"
#include "inet/physicallayer/common/packetlevel/SignalTag_m.h"
#include "inet/mobility/contract/IMobility.h"

namespace inet {

namespace physicallayer {

Define_Module(UnitDiskTransmitter);

UnitDiskTransmitter::UnitDiskTransmitter() :
    headerBitLength(-1),
    bitrate(NaN),
    maxCommunicationRange(NaN),
    maxInterferenceRange(NaN),
    maxDetectionRange(NaN)
{
}

void UnitDiskTransmitter::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        preambleDuration = par("preambleDuration");
        headerBitLength = par("headerBitLength");
        bitrate = bps(par("bitrate"));
        maxCommunicationRange = m(par("maxCommunicationRange"));
        maxInterferenceRange = m(par("maxInterferenceRange"));
        maxDetectionRange = m(par("maxDetectionRange"));
    }
}

std::ostream& UnitDiskTransmitter::printToStream(std::ostream& stream, int level) const
{
    stream << "UnitDiskTransmitter";
    if (level >= PRINT_LEVEL_TRACE)
        stream << ", preambleDuration = " << preambleDuration
               << ", headerBitLength = " << headerBitLength
               << ", bitrate = " << bitrate;
    if (level >= PRINT_LEVEL_INFO)
        stream << ", maxCommunicationRange = " << maxCommunicationRange;
    if (level >= PRINT_LEVEL_TRACE)
        stream << ", maxInterferenceRange = " << maxInterferenceRange
               << ", maxDetectionRange = " << maxDetectionRange;
    return stream;
}

const ITransmission *UnitDiskTransmitter::createTransmission(const IRadio *transmitter, const cPacket *macFrame, const simtime_t startTime) const
{
    auto signalBitrateReq = const_cast<cPacket *>(macFrame)->getTag<SignalBitrateReq>();
    auto transmissionBitrate = signalBitrateReq != nullptr ? signalBitrateReq->getDataBitrate() : bitrate;
    auto headerDuration = headerBitLength / transmissionBitrate.get();
    auto dataDuration = macFrame->getBitLength() / transmissionBitrate.get();
    auto duration = preambleDuration + headerDuration + dataDuration;
    auto endTime = startTime + duration;
    auto mobility = transmitter->getAntenna()->getMobility();
    auto startPosition = mobility->getCurrentPosition();
    auto endPosition = mobility->getCurrentPosition();
    auto startOrientation = mobility->getCurrentAngularPosition();
    auto endOrientation = mobility->getCurrentAngularPosition();
    return new UnitDiskTransmission(transmitter, macFrame, startTime, endTime, preambleDuration, headerDuration, dataDuration, startPosition, endPosition, startOrientation, endOrientation, maxCommunicationRange, maxInterferenceRange, maxDetectionRange);
}

} // namespace physicallayer

} // namespace inet

