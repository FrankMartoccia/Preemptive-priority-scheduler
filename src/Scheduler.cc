#include "Scheduler.h"

Define_Module(Scheduler);

void Scheduler::initialize() {
    processingTimerLow_ = new cMessage("processingTimerLow");
    processingTimerHigh_ = new cMessage("processingTimerHigh");

    logger = par("logger");

}

void Scheduler::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (logger) {
            EV << "scheduler: Job processed at time: " << simTime() << " with name: " << msg->getName() << endl;
        }
        if (strcmp(msg->getName(), "processingTimerHigh") == 0) {
            removeHighJob();
        } else {
            removeLowJob();
        }
    } else {
        Job* job = check_and_cast<Job*>(msg);
        job->setQueueArrival(simTime());
        if (logger) {
            EV << "scheduler: Job arrived at time: " << job->getQueueArrival() << endl;
            EV << "scheduler: Job service time: " << job->getServiceTime() << endl;
        }
        if (job->getIsHighPriority()) {
            highPriorityQueue.push(job);
        } else {
            lowPriorityQueue.push(job);
        }
        EV << "scheduler: Low Priority Queue Size: " << lowPriorityQueue.size() << endl;
        EV << "scheduler: High Priority Queue Size: " << highPriorityQueue.size() << endl;
        if (highPriorityQueue.size() == 1) {
            processHighJob();
        }
        if (highPriorityQueue.empty() && lowPriorityQueue.size() == 1) {
            processLowJob();
        }
    }
}


void Scheduler::processLowJob(){
    Job* job = lowPriorityQueue.front();
    scheduleAt(simTime() + job->getServiceTime(), processingTimerLow_);
}

void Scheduler::processHighJob(){
    Job* job = highPriorityQueue.front();
    scheduleAt(simTime() + job->getServiceTime(), processingTimerHigh_);
}

void Scheduler::removeHighJob(){
    highPriorityQueue.pop();
    if (logger) {
        EV << "scheduler: High priority job removed" << endl;
        EV << "scheduler: Low Priority Queue Size: " << lowPriorityQueue.size() << endl;
        EV << "scheduler: High Priority Queue Size: " << highPriorityQueue.size() << endl;
    }
}

void Scheduler::removeLowJob(){
    lowPriorityQueue.pop();
    if (logger) {
        EV << "scheduler: Low priority job removed" << endl;
        EV << "scheduler: Low Priority Queue Size: " << lowPriorityQueue.size() << endl;
        EV << "scheduler: High Priority Queue Size: " << highPriorityQueue.size() << endl;
    }
}
