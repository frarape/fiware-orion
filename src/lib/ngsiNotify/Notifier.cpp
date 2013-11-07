/*
*
* Copyright 2013 Telefonica Investigacion y Desarrollo, S.A.U
*
* This file is part of Orion Context Broker.
*
* Orion Context Broker is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Orion Context Broker is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
* General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Orion Context Broker. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* fermin at tid dot es
*
* Author: Fermin Galan
*/

#include "Notifier.h"

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "common/string.h"
#include "ngsi10/NotifyContextRequest.h"

#include "onTimeIntervalThread.h"
#include "senderThread.h"
#include "rest/clientSocketHttp.h"


/* ****************************************************************************
*
* Select 'method' to send notifications - only one can be uncommented
*/
//#define SEND_BLOCKING
#define SEND_IN_NEW_THREAD



/* ****************************************************************************
*
* ~Notifier -
*/
Notifier::~Notifier (void) {
    // FIXME: This destructor is needed to avoid warning message. 
    // Compilation fails when a warning occurs, and it is enabled 
    // compilation option -Werror "warnings being treated as errors" 
    LM_W(("not implemented Notifier destructor"));
}

/* ****************************************************************************
*
* Notifier::sendNotifyContextRequest -
*/
void Notifier::sendNotifyContextRequest(NotifyContextRequest* ncr, std::string url)
{
    /* Render NotifyContextRequest */
    // FIXME: currently, XML is hardwired as rendering format, but the subscriber
    // may want to receive JSON. How can we specify this? Maybe using metadata on
    // the subscribeContext?
    std::string payload = ncr->render(NotifyContext, XML, "");

    /* Parse URL */
    std::string  host;
    int          port;
    std::string  path;

    if (!parseUrl(url, host, port, path)) {
        LM_RVE(("Sending NotifyContextRequest: malformed URL: '%s'", url.c_str()));
    }


#ifdef SEND_BLOCKING
    sendHttpSocket(host, port, "POST", path, "application/xml", payload, false);
#endif

#ifdef SEND_IN_NEW_THREAD
    /* Send the message (no wait for response), in a separate thread to avoid blocking */
    pthread_t tid;
    SenderThreadParams* params = new SenderThreadParams();
    params->ip = host;
    params->port = port;
    params->verb = "POST";
    params->resource = path;
    //FIXME: unhardwire content type
    params->content_type = std::string ("application/xml");
    params->content = payload;

    int ret = pthread_create(&tid, NULL, startSenderThread, params);
    if (ret != 0) {
        LM_RVE(("error creating thread: %d", ret));
    }
    pthread_detach(tid);
#endif
}


/* ****************************************************************************
*
* Notifier::sendNotifyContextAvailabilityRequest -
*
* FIXME: this method is very similar to sendNotifyContextRequest and probably
* they could be refactored in the future to have a common part using a parent
* class for both types of notifications and using it as first argument
*/
void Notifier::sendNotifyContextAvailabilityRequest(NotifyContextAvailabilityRequest* ncar, std::string url) {

    /* Render NotifyContextAvailabilityRequest */
    // FIXME: currently, XML is hardwired as rendering format, but the subscriber
    // may want to receive JSON. How can we specify this? Maybe using metadata on
    // the subscribeContext?
    // With the new classes, the input and output format is a part of the NgsiBase class.
    std::string payload = ncar->render(NotifyContextAvailability, XML, "");

    /* Parse URL */
    std::string host;
    int port;
    std::string path;
    if (!parseUrl(url, host, port, path)) {
        LM_RVE(("Sending NotifyContextAvailabilityRequest: malformed URL: '%s'", url.c_str()));
    }

    /* Send the message (no wait for response, in a separated thread to avoid blocking response)*/
#ifdef SEND_BLOCKING
    sendHttpSocket(host, port, "POST", path, "application/xml", payload, false);
#endif

#ifdef SEND_IN_NEW_THREAD
    pthread_t tid;
    SenderThreadParams* params = new SenderThreadParams();

    params->ip       = host;
    params->port     = port;
    params->verb     = "POST";
    params->resource = path;
    //FIXME: unhardwire content type
    params->content_type = "application/xml";
    params->content      = payload;

    int ret = pthread_create(&tid, NULL, startSenderThread, params);
    if (ret != 0) {
        LM_RVE(("error creating thread: %d", ret));
    }
    pthread_detach(tid);
#endif
}

/* ****************************************************************************
*
* Notifier::createIntervalThread -
*/
void Notifier::createIntervalThread(std::string subId, int interval) {

    /* Create params dynamically. Note that the first action that thread does
     * if */
    ThreadData td;
    td.params = new OnIntervalThreadParams();
    td.params->subId = subId;
    td.params->interval = interval;
    td.params->notifier = this;

    int ret = pthread_create(&(td.tid), NULL, startOnIntervalThread, td.params);
    if (ret != 0) {
        LM_RVE(("error creating thread: %d", ret));
    }

    pthread_detach(td.tid);

    /* Put the id in the list associate to subId */
    LM_T(LmtNotifier, ("created thread: %lu", (unsigned long) td.tid));
    this->threadsMap.insert(std::pair<std::string,ThreadData>(subId,td));
}

/* ****************************************************************************
*
* Notifier::destroyOntimeIntervalThreads -
*/
void Notifier::destroyOntimeIntervalThreads(std::string subId) {

    std::vector<pthread_t> canceled;

    /* Get all the ThreadParams associated to the given subId. Inspired in
     * http://advancedcppwithexamples.blogspot.com.es/2009/04/example-of-c-multimap.html
     */
    std::pair<std::multimap<std::string, ThreadData>::iterator, std::multimap<std::string, ThreadData>::iterator> ii;
    std::multimap<std::string, ThreadData>::iterator it;
    ii = this->threadsMap.equal_range(subId);
    for (it = ii.first; it != ii.second; ++it) {

        ThreadData td = it->second;

        /* Destroy thread */        
        int ret = pthread_cancel(td.tid);
        if (ret != 0) {
            LM_RVE(("error canceling thread %lu: %d", (unsigned long) td.tid, ret));
        }

        /* Note that we do the cancelation in parallel, storing the thread ID. This
         * vector is processed afterwards to wait for every thread to finish */
        canceled.push_back(td.tid);

        /* Release memory */
        delete td.params;

    }

    /* Remove key from the hashmap */
    threadsMap.erase(subId);

    /* Wait for all the cancelation to end */
    for (unsigned int ix = 0; ix < canceled.size(); ++ix) {
        void* res;

        /* pthread_join in blocking */
        int ret = pthread_join(canceled[ix], &res);
        if (ret != 0) {
            LM_RVE(("error joining thread %lu: %d", (unsigned long) canceled[ix], ret));
        }

        if (res == PTHREAD_CANCELED) {
            LM_T(LmtNotifier, ("canceled thread: %lu", (unsigned long) canceled[ix]));
        }
        else {
            LM_RVE(("unexpected error: thread can not be canceled!"));
        }

    }

    canceled.clear();
}