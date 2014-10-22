/* C++ STL */
#include <cstdlib>
#include <iostream>
#include <list>
#include <utility>

/* FNCS headers */
#include "integrator.h"
#include "message.h"
#include "objectcomminterface.h"

using namespace std;
using namespace sim_comm;

#ifndef nullptr 
#define nullptr NULL
#endif

static TIME time_current=0;
/* time scheduled for delivery and the message to deliver */
static list<pair<TIME,Message*> > events;
static ObjectCommInterface *simObj1 = NULL;
static ObjectCommInterface *simObj2 = NULL;

TIME getCurrentTime() {
    return time_current;
}

/* callback when simObj1 shadow gets a message */
static void receive_message1()
{
    while (simObj1->hasMoreMessages()) {
        Message *message = simObj1->getNextInboxMessage();
        /* schedule when simObj2 should extract message from
         * simObj1 out of the network, 0 to 2 seconds later, randomly */
        events.push_back(make_pair(time_current+rand()%2000000000, message));
        cout << "NetSim: simObj1 got message with timestamp "
            << message->getTime()
            << ", scheduled for "
            << events.back().first << endl;
    }
    events.sort();
}

static void network_simulator()
{
    TIME time_granted=0;
    TIME time_desired=0;

    Integrator::initIntegrator("sim_network.json", time_current);
    Integrator::setTimeCallBack(CreateCallback(getCurrentTime));
    simObj1 = Integrator::getCommInterface("simObj1");
    simObj2 = Integrator::getCommInterface("simObj2");
    Integrator::finalizeRegistrations();

    simObj1->setMessageNotifier(CreateCallback(receive_message1));

    do {
        /* Process any events in our event queue first.
         * That way, any sent messages will be available to the other
         * simulator after the timeStepStart call. */
        while (!events.empty() && events.front().first <= time_granted) {
            Message *message = events.front().second;
            time_current = events.front().first;
            cout << "NetSim: event detected at " << time_current << endl;
            events.pop_front();
            /* send it on to the actual, non-shadow object! */
            simObj2->send(message);
        }
        time_current = time_granted;

        Integrator::timeStepStart(time_current);
        cout << "NetSim: Working. Time is " << time_current << endl;

        if (events.empty()) {
            time_desired = Infinity;
        }
        else {
            time_desired = events.front().first;
        }

        time_granted = Integrator::getNextTime(time_current, time_desired);

        cout << "NetSim: getNextTime"
            << " current=" << time_current
            << " desired=" << time_desired
            << " granted=" << time_granted << endl;
    }
    while (!Integrator::isFinished());
    cout << "NetSim done!" << endl;

    Integrator::stopIntegrator();
}


int main(int argc, char **argv)
{
    try {
        cout << "starting network sim" << endl;
        network_simulator();
    } catch (const std::exception &e) {
        cout << e.what() << endl;
    }
    catch (const string &e) {
        cout << e << endl;
    }
    cout << "finished network sim" << endl;
    return 0;
}
