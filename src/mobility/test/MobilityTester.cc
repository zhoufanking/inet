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
#include <fstream>

#include "MobilityTester.h"

Define_Module(MobilityTester);

MobilityTester::MobilityTester()
{
    updateTimer = NULL;
    updateInterval = 0;
}
MobilityTester::~MobilityTester()
{
    cancelAndDelete(updateTimer);
}
void MobilityTester::initialize()
{
    mobilityModule = check_and_cast<MobilityBase*>(getModuleByPath(par("mobilityPath")));
    updateTimer = new cMessage("update");
    updateInterval = par("updateInterval");
    testInterval = par("testInterval");
    scheduleAt(simTime() + updateInterval, updateTimer);
}

void MobilityTester::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        update();
    else
        throw cRuntimeError("Mobility modules can only receive self messages");
}

void MobilityTester::update()
{
    if (simTime() != testInterval)
    {
        if (updateInterval != 0 && simTime() + updateInterval < testInterval)
        {
            scheduleAt(simTime() + updateInterval, updateTimer);
            EV_DEBUG << mobilityModule->getCurrentPosition() << endl;
            EV_DEBUG << mobilityModule->getCurrentSpeed() << endl;
        }
        else
        {
            scheduleAt(testInterval, updateTimer);
        }
    }
    else
    {
        char log_file_name[70];
        std::ofstream logfile;
        sprintf(log_file_name, "%s.txt", mobilityModule->getClassName());//, updateInterval.str().c_str());
        logfile.open (log_file_name, std::fstream::app);
        logfile << updateInterval << endl;
        logfile << mobilityModule->getCurrentPosition() << endl;
        logfile << mobilityModule->getCurrentSpeed() << endl;
        logfile.close();
    }
}

void MobilityTester::finish()
{
}

