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
#include "logMsg/traceLevels.h"
#include "common/globals.h"

#include "convenienceMap/mapPostContextEntityAttributes.h"
#include "convenienceMap/mapGetContextEntityAttributes.h"
#include "convenience/RegisterProviderRequest.h"
#include "convenienceMap/mapPostIndividualContextEntity.h"
#include "mongoBackend/mongoRegisterContext.h"
#include "ngsi9/RegisterContextResponse.h"
#include "ngsi9/DiscoverContextAvailabilityResponse.h"
#include "testInit.h"
#include "commonMocks.h"



using ::testing::_;
using ::testing::Throw;
using ::testing::Return;



/* ****************************************************************************
*
* prepareDatabase -
*/
static void prepareDatabase(std::string id, std::string type)
{
  static DBClientConnection* connection = NULL;

  if (connection == NULL)
  {
    /* Set database */
    setupDatabase();

    connection = getMongoConnection();
  }

  /* Create one entity:
   *
   * - 'id', 'type' with four attributes
   *     A1: X
   *     A1: Y
   *     A2: Z
   *     A3: W
   */

  BSONObj en1 = BSON("_id" << BSON("id" << id << "type" << type) <<
                     "attrs" << BSON_ARRAY(
                        BSON("name" << "A1" << "type" << "TA1" << "value" << "X") <<
                        BSON("name" << "A1" << "type" << "TA1bis" << "value" << "Y") <<
                        BSON("name" << "A2" << "type" << "TA2" << "value" << "Z") <<
                        BSON("name" << "A3" << "type" << "TA3" << "value" << "W")
                        )
     );

  connection->insert(ENTITIES_COLL, en1);
}



/* ****************************************************************************
*
* notFoundThenFound - 
*/
TEST(mapPostIndividualContextEntity, notFoundThenFound)
{
  HttpStatusCode                ms;
  AppendContextElementRequest   request;
  AppendContextElementResponse  response;

  /* Set timer */
  Timer* t = new Timer();
  setTimer(t);

  prepareDatabase("MPICE", "ttt");
  request.attributeDomainName.set("ad");

  ms = mapPostIndividualContextEntity("MPICE", &request, &response);
  EXPECT_EQ(SccOk, ms);
  EXPECT_EQ(200, response.errorCode.code);

  ms = mapPostIndividualContextEntity("MPICE2", &request, &response);
  EXPECT_EQ(SccOk, ms);
  EXPECT_EQ(404, response.errorCode.code);

  prepareDatabase("MPICE", "ttt2");
  ms = mapPostIndividualContextEntity("MPICE", &request, &response);
  EXPECT_EQ(SccOk, ms);
  EXPECT_EQ(500, response.errorCode.code);
  EXPECT_STREQ("Bad size of contextElementResponseVector from mongoUpdateContext", response.errorCode.details.c_str());

  ms = mapPostIndividualContextEntity("NADA", &request, &response);
  EXPECT_EQ(SccOk, ms);
  EXPECT_EQ(404, response.errorCode.code);
  EXPECT_STREQ("Entity not found", response.errorCode.reasonPhrase.c_str());
}