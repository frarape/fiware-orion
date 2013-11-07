#ifndef REGISTER_CONTEXT_RESPONSE_H
#define REGISTER_CONTEXT_RESPONSE_H

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
#include <string>

#include "ngsi/ErrorCode.h"
#include "ngsi/RegistrationId.h"
#include "ngsi/Duration.h"
#include "common/Format.h"
#include "ngsi9/RegisterContextRequest.h"



/* ****************************************************************************
*
* RegisterContextResponse - 
*/
typedef struct RegisterContextResponse
{
  Duration        duration;         // Optional
  RegistrationId  registrationId;   // Mandatory
  ErrorCode       errorCode;        // Optional

  RegisterContextResponse();
  ~RegisterContextResponse();
  RegisterContextResponse(RegisterContextRequest* rcrP);
  RegisterContextResponse(std::string _registrationId, std::string _duration);
  RegisterContextResponse(std::string _registrationId, ErrorCode& _errorCode);

  std::string render(RequestType requestType, Format format, std::string indent);
  std::string check(RequestType requestType, Format format, std::string indent, std::string predetectedError, int counter);
  void        present(std::string indent);
  void        release(void);
} RegisterContextResponse;

#endif