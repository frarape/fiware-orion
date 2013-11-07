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
* Author: Ken Zangelin
*/
#include "gtest/gtest.h"

#include "logMsg/logMsg.h"

#include "serviceRoutines/postUpdateContextAvailabilitySubscription.h"
#include "serviceRoutines/badRequest.h"
#include "rest/RestService.h"

#include "testDataFromFile.h"



/* ****************************************************************************
*
* rs - 
*/
static RestService rs[] = 
{
  { "POST UpdateContextAvailabilitySubscription", "POST",   UpdateContextAvailabilitySubscription, 2, { "ngsi9",  "updateContextAvailabilitySubscription"    }, "", postUpdateContextAvailabilitySubscription },
  { "* InvalidRequest",                           "*",      InvalidRequest,                        0, { "*", "*", "*", "*", "*", "*"                         }, "", badRequest                                },
  { "* *",                                        "",       InvalidRequest,                        0, {                                                      }, "", NULL                                      }
};



/* ****************************************************************************
*
* notFound - 
*/
TEST(postUpdateContextAvailabilitySubscription, notFound)
{
  ConnectionInfo ci("/ngsi9/updateContextAvailabilitySubscription",  "POST", "1.1");
  std::string    expected    = "<updateContextAvailabilitySubscriptionResponse>\n  <errorCode>\n    <code>404</code>\n    <reasonPhrase>Subscription Not Found</reasonPhrase>\n  </errorCode>\n  <subscriptionId>111122223333444455556666</subscriptionId>\n</updateContextAvailabilitySubscriptionResponse>\n";
  const char*    fileName    = "updateContextAvailabilitySubscription_notFound.xml";
  std::string    out;

  EXPECT_EQ("OK", testDataFromFile(testBuf, sizeof(testBuf), fileName)) << "Error getting test data from '" << fileName << "'";

  ci.outFormat    = XML;
  ci.inFormat     = XML;
  ci.payload      = testBuf;
  ci.payloadSize  = strlen(testBuf);
  out             = restService(&ci, rs);

  EXPECT_STREQ(expected.c_str(), out.c_str());
}