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
#include <stdio.h>
#include <string>

#include "common/globals.h"
#include "common/tag.h"
#include "ngsi/Request.h"
#include "ngsi/NotifyCondition.h"



/* ****************************************************************************
*
* NotifyCondition::render - 
*/
std::string NotifyCondition::render(Format format, std::string indent)
{
  std::string out = "";
  std::string tag = "notifyCondition";

  out += startTag(indent, tag, format);
  out += valueTag(indent + "  ", "type", type, format);
  out += condValueList.render(format, indent + "  ");
  out += restriction.render(format, indent + "  ");
  out += endTag(indent, tag, format);

  return out;
}



/* ****************************************************************************
*
* NotifyCondition::check - 
*/
std::string NotifyCondition::check(RequestType requestType, Format format, std::string indent, std::string predetectedError, int counter)
{
  std::string res;

  if (type == "")
    return "empty type for NotifyCondition";
  else if (strcasecmp(type.c_str(), "ONTIMEINTERVAL") == 0)
    ;
  else if (strcasecmp(type.c_str(), "ONCHANGE") == 0)
    ;
  else if (strcasecmp(type.c_str(), "ONVALUE") == 0)
    ;
  else
    return std::string("invalid notify condition type: '") + type + "'";

  if ((res = condValueList.check(requestType, format, indent, predetectedError, counter)) != "OK") return res;
  if ((res = restriction.check(requestType, format, indent, predetectedError, counter)) != "OK")   return res;
  
  return "OK";
}



/* ****************************************************************************
*
* NotifyCondition::present - 
*/
void NotifyCondition::present(std::string indent, int ix)
{
  std::string indent2 = indent + "  ";

  if (ix == -1)
    PRINTF("%sNotify Condition:\n", indent2.c_str());
  else
    PRINTF("%sNotify Condition %d:\n", indent2.c_str(), ix);

  PRINTF("%stype: %s\n", indent2.c_str(), type.c_str());
  condValueList.present(indent2);
  restriction.present(indent2);
}



/* ****************************************************************************
*
* NotifyCondition::release - 
*/
void NotifyCondition::release(void)
{
  condValueList.release();
}