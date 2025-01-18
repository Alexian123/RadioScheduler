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

#include "Source.h"

Define_Module(Source);

int Source::totalPacketsGenerated = 0;

Source::Source()
{
    sendMessageEvent = nullptr;
    networkLoad = 0;
}

Source::~Source()
{
    cancelAndDelete(sendMessageEvent);
}

double Source::getNetworkLoad()
{
    return networkLoad;
}

void Source::initialize()
{
    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg == sendMessageEvent);
    cMessage *job = new cMessage("job");
    int packetLength = par("packetLength").intValue();
    job->addPar("packetLength").setLongValue(packetLength);
    send(job, "txPackets");

    totalPacketsGenerated++;

    cModule *user = getParentModule();
    cModule *scheduler = user->getParentModule()->getSubmodule("scheduler");

    double currentTime = simTime().dbl();
    double sendingTime = 0.0;

    if (currentTime != 0.0) {
        // Calculate network load
        double schedulingPeriod = scheduler->par("schedulingPeriod").doubleValue();
        int numChannels = scheduler->par("numberOfChannels").intValue();
        networkLoad = (totalPacketsGenerated * schedulingPeriod) / (currentTime * numChannels);

        // Calculate sendingTime
        int numUsers = scheduler->par("gateSize").intValue();
        int numPackets = user->par("weight");
        sendingTime = (numUsers * numPackets * packetLength * schedulingPeriod) / (networkLoad * numChannels);
    } else {
        sendingTime = par("sendIaTime").doubleValue();
    }

    EV << "Network Load: " << networkLoad << endl;
    EV << "Total Packets Generated: " << totalPacketsGenerated << endl;
    EV << "Current Time: " << currentTime << endl;
    EV << "Sending Time: " << sendingTime << endl;

    scheduleAt(simTime() + exponential(sendingTime), sendMessageEvent);
}
