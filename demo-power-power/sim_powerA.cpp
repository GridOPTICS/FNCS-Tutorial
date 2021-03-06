/* C++ STL */
#include <iostream>

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

/* the callback function that FNCS can use to query the curren time */
TIME getCurrentTime() {
    return time_current;
}

/* our simulator function, called by the main() function later */
static void generic_simulator()
{
    TIME time_granted=0; /* the time step FNCS has allowed us to process */
    TIME time_desired=0; /* the time step we would like to go to next */

    /* initialize FNCS integrator with configuration file and current time */
    Integrator::initIntegrator("sim_power.json", time_current);
    /* FNCS must be able to request the current time of the simulator */
    Integrator::setTimeCallBack(CreateCallback(getCurrentTime));
    /* register an object that wishes to communicate */
    ObjectCommInterface *simObjA = Integrator::getCommInterface("simObjA");
    /* it's required to tell FNCS when all registrations are complete */
    Integrator::finalizeRegistrations();

    /* unless FNCS detects another simulator terminates early using the
     * ::isFinished() function, this simulator will run from time step 0
     * to time step 9 */
    while (!Integrator::isFinished() && time_current < 10) {
        /* FNCS uses two-phase synchronization at each time step, once
         * at the beginning of the time step ("timeStepStart") and once
         * at the end ("getNextTime") */
        Integrator::timeStepStart(time_current);

        /* do useful work; check for incoming messages to the objects we
         * registered earlier */
        {
            cout << "PowerA: Working. Time is " << time_current << endl;

            /* Send empty (NULL, size 0) message from simObjA to simObjB. */
            Message *outgoing_message = new Message(
                    /* from */ "simObjA", 
                    /* to */ "simObjB",
                    /* timeStamp */ time_current,
                    /* buffer */ NULL,
                    /* buffer size */ 0,
                    /* tag value, defaults to 0 */ 0);

            /* By default, all messages are 'routed' through
             * the network simulator unless the 'delayThroughComm'
             * attribute is set to false. */
            outgoing_message->setDelayThroughComm(false);

            /* actually send the message */
            simObjA->send(outgoing_message);
            cout << "PowerA: sent message from simObjA to simObjB" << endl;

            /* Check for incoming messages. */
            while (simObjA->hasMoreMessages()) {
                Message *incoming_message = simObjA->getNextInboxMessage();
                cout << "PowerA: simObjA received message from "
                    << incoming_message->getFrom()
                    << " with timestamp "
                    << incoming_message->getTime() << endl;
                delete incoming_message;
            }
            /* Which time step do we wish to go to next? This does not
             * necessarily need to be a delta of 1; FNCS supports
             * arbitrary time deltas. */
            time_desired = time_current + 1;

            /* No need to delete outgoing message object;
             * FNCS takes ownership. */
        }

        /* Second phase of two-phase synchronization by requesting the
         * next time step. The next time could very well be smaller
         * than this simulator is able to process i.e. the network
         * simulator runs in nanoseconds while this simulator runs in
         * seconds. In such a case, the time_granted will be the same as
         * time_current. Another possibility is that the time_granted is
         * less than time_desired due to another simulator requesting
         * a smaller time delta. */
        time_granted = Integrator::getNextTime(time_current, time_desired);

        cout << "PowerA: getNextTime"
            << " current=" << time_current
            << " desired=" << time_desired
            << " granted=" << time_granted << endl;

        time_current = time_granted;
    }
    cout << "PowerA: DONE!" << endl;

    Integrator::stopIntegrator();
}


/* a simple main that calls our simulator function;
 * it is sometimes useful to capture and report exceptions but this
 * might be overkill for your particular needs */
int main(int argc, char **argv)
{
    try {
        cout << "starting generic simulator" << endl;
        generic_simulator();
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    catch (const string &e) {
        cout << e << endl;
    }
    cout << "finished generic simulator" << endl;
    return 0;
}
