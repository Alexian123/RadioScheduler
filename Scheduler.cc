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

#include "Scheduler.h"

Define_Module(Scheduler);

Scheduler::Scheduler()
{
    selfMsg = nullptr;
}

Scheduler::~Scheduler()
{
    cancelAndDelete(selfMsg);
}


void Scheduler::initialize()
{
    numUsers = par("gateSize").intValue();
    numChannels = par("numberOfChannels").intValue();
    selfMsg = new cMessage("selfMsg");
    for (int i = 0; i < MAX_NUM_USERS; i++) {
           queueLengths[i] = 0;
           numBlocks[i] = 0;
    }
    scheduleAt(simTime(), selfMsg);
}

void Scheduler::handleMessage(cMessage *msg)
{
    for (int i = 0; i < numUsers; i++) {
        if (msg->arrivedOn("rxInfo", i)) {
            queueLengths[i] = msg->par("ql_info");
            EV << "Update: queueLengths[" << i << "]= " << queueLengths[i] << endl;
            delete(msg);
        }
    }

    if (msg == selfMsg) {
        int remainingChannels = numChannels; // Start with all channels
        int currentUser = 0;  // Start with the first user

        while (remainingChannels > 0 && currentUser < numUsers) {
            int userWeight = getParentModule()->getSubmodule("user", currentUser)->par("weight").intValue();
            numBlocks[currentUser] = std::min(remainingChannels, std::min(userWeight, queueLengths[currentUser]));
            remainingChannels -= numBlocks[currentUser];
            currentUser++;
        }

        for (int i = 0; i < numUsers; i++) {
            EV << "Update: numBlocks[" << i << "]=" << numBlocks[i] << endl;
            if (numBlocks[i] > 0) {
                cMessage *cmd = new cMessage("cmd");
                cmd->addPar("numBlocks");
                cmd->par("numBlocks").setLongValue(numBlocks[i]);
                send(cmd, "txScheduling", i);
            }
        }

        scheduleAt(simTime()+par("schedulingPeriod").doubleValue(), selfMsg);
    }

}
