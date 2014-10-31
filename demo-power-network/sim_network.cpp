/* C++ STL */
#include <cstdlib>
#include <iostream>
#include <list>
#include <utility>

/* FNCS headers */
#include "integrator.h"
#include "message.h"
#include "objectcomminterface.h"

using namespace std; /* C++ standard namespace */
using namespace sim_comm; /* the FNCS namespace */

/* some C++ compilers have a nullptr instance,
 * otherwise we fall back to the old default of NULL */
#ifndef nullptr 
#define nullptr NULL
#endif

/* the current time of our simulator */
static TIME time_current=0;
/* time scheduled for delivery and the message to deliver */
static list<pair<TIME,Message*> > events;
/* the entitites that will communicate */
static ObjectCommInterface *simObj1 = NULL;
static ObjectCommInterface *simObj2 = NULL;

/* the callback function that FNCS can use to query the curren time */
TIME getCurrentTime() {
    return time_current;
}

/* callback when simObj1 shadow gets a message */
static void receive_message1()
{
    /* Just because FNCS will call this callback function to indicate
     * there are messages in the inbox, that doesn't mean they are
     * automatically processed. You must process all incoming messages
     * for this object. */
    while (simObj1->hasMoreMessages()) {
        Message *message = simObj1->getNextInboxMessage();
        /* Schedule when simObj2 should extract message from
         * simObj1 out of the network, 0 to 2 seconds later, randomly.
         * This is a very crude network simulator, randomly delaying
         * messages. */
        events.push_back(make_pair(time_current+rand()%2000000000, message));
        cout << "NetSim: simObj1 got message with timestamp "
            << message->getTime()
            << ", scheduled for "
            << events.back().first << endl;
    }
    /* Event-based simulators maintain a sorted event queue. */
    events.sort();
}

/* our simulator function, called by the main() function later */
static void network_simulator()
{
    TIME time_granted=0;
    TIME time_desired=0;

    /* initialize FNCS integrator with configuration file and current time */
    Integrator::initIntegrator("sim_network.json", time_current);
    /* FNCS must be able to request the current time of the simulator */
    Integrator::setTimeCallBack(CreateCallback(getCurrentTime));
    /* register an object that wishes to communicate */
    simObj1 = Integrator::getCommInterface("simObj1");
    /* register an object that wishes to communicate */
    simObj2 = Integrator::getCommInterface("simObj2");
    /* it's required to tell FNCS when all registrations are complete */
    Integrator::finalizeRegistrations();

    /* register callback function for simObj1 for incoming messages */
    simObj1->setMessageNotifier(CreateCallback(receive_message1));

    /* this network simulator will run indefinitely until FNCS detects
     * that the rest of the simulators are finished */
    do {
        /* Process any events in our event queue first.
         * That way, any sent messages will be available to the other
         * simulator after the timeStepStart call. */
        while (!events.empty() && events.front().first <= time_granted) {
            /* We have an event scheduled and it's now time to send it.
             * Pull it off the event queue and send it. */
            Message *message = events.front().second;
            time_current = events.front().first;
            cout << "NetSim: event detected at " << time_current << endl;
            events.pop_front();
            /* send it on to the actual, non-shadow object! */
            simObj2->send(message);
        }
        time_current = time_granted;

        /* Tell FNCS we are processing the current time step. */
        Integrator::timeStepStart(time_current);
        cout << "NetSim: Working. Time is " << time_current << endl;

        /* If we have more events, we can tell FNCS the time of the next
         * event (time_desired). Otherwise, we participate in the
         * synchronization call but our vote of Infinity essentially
         * doesn't affect the outcome. 'Infinity' means the simulator is
         * in a steady state. */
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


/* a simple main that calls our simulator function;
 * it is sometimes useful to capture and report exceptions but this
 * might be overkill for your particular needs */
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
