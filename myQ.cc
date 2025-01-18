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

#include "myQ.h"

Define_Module(MyQ);

void MyQ::initialize()
{
    queue.setName("queue");
}

void MyQ::handleMessage(cMessage *msg)
{
    int queueLengthParam = par("queueLength").intValue();
    if (msg->arrivedOn("rxPackets")) {
        if (queue.getLength() < queueLengthParam) { // Check if queue is full
            queue.insert(msg);
        } else {
            EV << "Queue is full, dropping packet." << endl;
            delete msg;
        }
    } else if (msg->arrivedOn("rxScheduling")) {
        delete msg;
        while (!queue.isEmpty()) {
          msg = (cMessage*)queue.pop();
          send(msg, "txPackets");
        }
    }
    int queueLength = queue.getLength();

    cMessage *qInfo = new cMessage("qInfo");
    qInfo->addPar("ql_info");
    qInfo->par("ql_info").setLongValue(queueLength);

    int i = getParentModule()->getIndex();

    long ql_info_tst = qInfo->par("ql_info");
    EV << "queue[" << i << "] = " << queueLength << " ql_info_tst = " << ql_info_tst << endl;
    send(qInfo, "txInfo");
}
