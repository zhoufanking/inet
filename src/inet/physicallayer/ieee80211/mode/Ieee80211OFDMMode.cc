//
// Copyright (C) 2014 OpenSim Ltd.
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

#include "inet/physicallayer/ieee80211/mode/Ieee80211OFDMMode.h"
#include "inet/physicallayer/ieee80211/layered/Ieee80211OFDMDefs.h"
#include "inet/physicallayer/ieee80211/Ieee80211OFDMModulation.h"

namespace inet {

namespace physicallayer {

bps Ieee80211OFDMModeBase::computeGrossBitrate(const Ieee80211OFDMModulation *modulation) const
{
    int codedBitsPerOFDMSymbol = modulation->getModulation()->getCodeWordSize() * NUMBER_OF_OFDM_DATA_SUBCARRIERS;
    double dataBitsPerOFDMSymbol = codedBitsPerOFDMSymbol;
    simtime_t symbolDuration = getSymbolInterval();
    return bps(dataBitsPerOFDMSymbol / symbolDuration);
}

bps Ieee80211OFDMModeBase::computeNetBitrate(bps grossBitrate, const Ieee80211OFDMCode* code) const
{
    const ConvolutionalCode *convolutionalCode = code ? code->getConvolutionalCode() : nullptr;
    if (convolutionalCode)
        return bps(convolutionalCode->getCodeRatePuncturingK() * grossBitrate.get() / convolutionalCode->getCodeRatePuncturingN());
    return grossBitrate;
}


Ieee80211OFDMMode::Ieee80211OFDMMode(const Ieee80211OFDMPreambleMode *preambleMode, const Ieee80211OFDMSignalMode* signalMode, const Ieee80211OFDMDataMode* dataMode, Hz channelSpacing, Hz bandwidth) :
        Ieee80211OFDMModeBase(channelSpacing, bandwidth),
        preambleMode(preambleMode),
        signalMode(signalMode),
        dataMode(dataMode)
{
}

Ieee80211OFDMSignalMode::Ieee80211OFDMSignalMode(const Ieee80211OFDMCode* code, const Ieee80211OFDMModulation* modulation, Hz channelSpacing, Hz bandwidth, unsigned int rate) :
        Ieee80211OFDMModeBase(channelSpacing, bandwidth),
        code(code),
        modulation(modulation),
        rate(rate)
{
    grossBitrate = computeGrossBitrate(modulation);
    netBitrate = computeNetBitrate(grossBitrate, code);
}

Ieee80211OFDMDataMode::Ieee80211OFDMDataMode(const Ieee80211OFDMCode* code, const Ieee80211OFDMModulation* modulation, Hz channelSpacing, Hz bandwidth) :
        Ieee80211OFDMModeBase(channelSpacing, bandwidth),
        code(code),
        modulation(modulation)
{
    grossBitrate = computeGrossBitrate(modulation);
    netBitrate = computeNetBitrate(grossBitrate, code);
}

Ieee80211OFDMModeBase::Ieee80211OFDMModeBase(Hz channelSpacing, Hz bandwidth) :
        channelSpacing(bandwidth),
        bandwidth(bandwidth)
{
}

Ieee80211OFDMPreambleMode::Ieee80211OFDMPreambleMode(Hz channelSpacing, Hz bandwidth) :
        Ieee80211OFDMModeBase(channelSpacing, bandwidth)
{
}

const Ieee80211OFDMMode& Ieee80211OFDMCompliantModes::getCompliantMode(unsigned int signalRateField, Hz channelSpacing)
{
    // Table 18-6—Contents of the SIGNAL field
    if (channelSpacing == MHz(20))
    {
        switch (signalRateField)
        {
        case 13: // 1101
            return ofdmMode6MbpsCS20MHz;
        case 15: // 1111
            return ofdmMode9MbpsCS20MHz;
        case 5:  // 0101
            return ofdmMode12MbpsCS20MHz;
        case 7:  // 0111
            return ofdmMode18MbpsCS20MHz;
        case 9:  // 1001
            return ofdmMode24MbpsCS20MHz;
        case 11: // 1011
            return ofdmMode36Mbps;
        case 1:  // 0001
            return ofdmMode48Mbps;
        case 3:  // 0011
            return ofdmMode54Mbps;
        default:
            throw cRuntimeError("%d is not a valid rate", signalRateField);
        }
    }
    else if (channelSpacing == MHz(10))
    {
        switch (signalRateField)
        {
        case 13:
            return ofdmMode3MbpsCS10MHz;
        case 15:
            return ofdmMode4_5MbpsCS10MHz;
        case 5:
            return ofdmMode6MbpsCS10MHz;
        case 7:
            return ofdmMode9MbpsCS10MHz;
        case 9:
            return ofdmMode12MbpsCS10MHz;
        case 11:
            return ofdmMode18MbpsCS10MHz;
        case 1:
            return ofdmMode24MbpsCS10MHz;
        case 3:
            return ofdmMode27Mbps;
        default:
            throw cRuntimeError("%d is not a valid rate", signalRateField);
        }
    }
    else if (channelSpacing == MHz(5))
    {
        switch (signalRateField)
        {
        case 13:
            return ofdmMode1_5Mbps;
        case 15:
            return ofdmMode2_25Mbps;
        case 5:
            return ofdmMode3MbpsCS5MHz;
        case 7:
            return ofdmMode4_5MbpsCS5MHz;
        case 9:
            return ofdmMode6MbpsCS5MHz;
        case 11:
            return ofdmMode9MbpsCS5MHz;
        case 1:
            return ofdmMode12MbpsCS5MHz;
        case 3:
            return ofdmMode13_5Mbps;
        default:
            throw cRuntimeError("%d is not a valid rate", signalRateField);
        }
    }
    else
        throw cRuntimeError("Channel spacing = %f must be 5, 10 or 20 MHz", channelSpacing.get());
}

// Preamble modes
const Ieee80211OFDMPreambleMode Ieee80211OFDMCompliantModes::ofdmPreambleModeCS5MHz(MHz(5), MHz(20));
const Ieee80211OFDMPreambleMode Ieee80211OFDMCompliantModes::ofdmPreambleModeCS10MHz(MHz(10), MHz(20));
const Ieee80211OFDMPreambleMode Ieee80211OFDMCompliantModes::ofdmPreambleModeCS20MHz(MHz(20), MHz(20));

//Ieee80211OFDMSignalMode(const Ieee80211OFDMCode *code, const Ieee80211OFDMModulation *modulation, Hz channelSpacing, Hz bandwidth, unsigned int rate);
// Signal Modes
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate13(MHz(20), MHz(20), 13);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate15(MHz(20), MHz(20), 15);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate5(MHz(20), MHz(20), 5);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate7(MHz(20), MHz(20), 7);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate9(MHz(20), MHz(20), 9);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate11(MHz(20), MHz(20), 11);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate1(MHz(20), MHz(20), 1);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode6MbpsRate3(MHz(20), MHz(20), 3);
//
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate13(MHz(10), MHz(20), 13);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate15(MHz(10), MHz(20), 15);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate5(MHz(10), MHz(20), 5);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate7(MHz(10), MHz(20), 7);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate9(MHz(10), MHz(20), 9);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate11(MHz(10), MHz(20), 11);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate1(MHz(10), MHz(20), 1);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode3MbpsRate3(MHz(10), MHz(20), 3);
//
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate13(MHz(5), MHz(20), 13);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate15(MHz(5), MHz(20), 15);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate5(MHz(5), MHz(20), 5);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate7(MHz(5), MHz(20), 7);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate9(MHz(5), MHz(20), 9);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate11(MHz(5), MHz(20), 11);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate1(MHz(5), MHz(20), 1);
//const Ieee80211OFDMSignalMode Ieee80211OFDMCompliantModes::ofdmHeaderMode1_5MbpsRate3(MHz(5), MHz(20), 3);

} // namespace physicallayer

} // namespace inet
