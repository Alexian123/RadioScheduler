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

#include "Sink.h"
#include "Source.h"

#include <string>
#include <fstream>

Define_Module(Sink);

void Sink::initialize()
{
    numUsers = par("gateSize");
    for (int i = 0; i < numUsers; i++) {
        sumDelay[i] = 0;
        minDelay[i] = SIMTIME_MAX;
        maxDelay[i] = 0;
        packetCount[i] = 0;
        cModule *user = getParentModule()->getSubmodule("user", i);
        int weight = user->par("weight").intValue();
        std::string fileName = "simulation_data_for_weight_" +  std::to_string(weight) + ".csv";
        std::ofstream outputFile(fileName);
        outputFile << "Time,NetworkLoad,Delay" << std::endl;
        outputFile.close();
    }
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;

    cModule *user = msg->getSenderModule()->getParentModule();
    int weight = user->par("weight").intValue();
    std::string fileName = "simulation_data_for_weight_" +  std::to_string(weight) + ".csv";
    std::ofstream outputFile(fileName, std::ios_base::app);

    int userIndex = user->getIndex();
    sumDelay[userIndex] += lifetime;
    minDelay[userIndex] = std::min(minDelay[userIndex], lifetime);
    maxDelay[userIndex] = std::max(maxDelay[userIndex], lifetime);
    packetCount[userIndex]++;

    int userWeight = user->par("weight").intValue();
    long packetLength = msg->par("packetLength").longValue();

    cModule *generator = user->getSubmodule("generator");
    Source *source = check_and_cast<Source*>(generator);
    double networkLoad = source->getNetworkLoad();

    outputFile << simTime().dbl() << "," << networkLoad << "," << lifetime << std::endl;
    outputFile.close();

    delete msg;
}

void Sink::finish()
{
    for (int i = 0; i < numUsers; i++) {
        cModule *user = getParentModule()->getSubmodule("user", i);
        int weight = user->par("weight").intValue();
        std::string fileName = "simulation_results_for_weight_" +  std::to_string(weight) + ".csv";
        std::ofstream outputFile(fileName);
        double averageDelay = 0;
        if (packetCount[i] > 0) {
            averageDelay = sumDelay[i].dbl() / packetCount[i];
        }
        outputFile << "Average delay: " << averageDelay << endl;
        outputFile << "Minimum delay: " << minDelay[i] << endl;
        outputFile << "Maximum delay: " << maxDelay[i] << endl;
        outputFile.close();
    }



}
