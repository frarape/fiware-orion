#ifndef UPDATE_CONTEXT_REQUEST_H
#define UPDATE_CONTEXT_REQUEST_H

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
* iot_support at tid dot es
*
* Author: Ken Zangelin
*/
#include <string>
#include <vector>

#include "common/Format.h"
#include "ngsi/ContextElementVector.h"
#include "ngsi/UpdateActionType.h"
#include "rest/ConnectionInfo.h"


/* ****************************************************************************
*
* Forward declarations
*/
struct UpdateContextElementRequest;
struct AppendContextElementRequest;



/* ****************************************************************************
*
* UpdateContextRequest - 
*/
typedef struct UpdateContextRequest
{
  ContextElementVector    contextElementVector;  // Mandatory
  UpdateActionType        updateActionType;      // Mandatory

  void         init(void);
  std::string  render(ConnectionInfo* ciP, RequestType requestType, const std::string& indent);
  std::string  check(ConnectionInfo* ciP,  RequestType requestType, const std::string& indent, const std::string& predetectedError, int counter);
  void         release(void);
  void         present(const std::string& indent);
  void         fill(const UpdateContextElementRequest* ucerP, const std::string& entityId, const std::string& entityType);
  void         fill(const AppendContextElementRequest* acerP);
  void         fill(const std::string& entityId, const std::string& entityType, const std::string& isPattern, const std::string& _updateActionType);
} UpdateContextRequest;

#endif
