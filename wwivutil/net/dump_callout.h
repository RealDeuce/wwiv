/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*             Copyright (C)1998-2016, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/**************************************************************************/
#ifndef __INCLUDED_WWIVUTIL_DUMP_CALLOUT_H__
#define __INCLUDED_WWIVUTIL_DUMP_CALLOUT_H__

#include <map>
#include <string>

#include "core/command_line.h"
#include "sdk/callout.h"
#include "wwivutil/command.h"

namespace wwiv {
namespace wwivutil {

class DumpCalloutCommand final: public UtilCommand {
public:
  DumpCalloutCommand()
    : UtilCommand("dump_callout", "Dumps parsed representation of CALLOUT.NET") {}
  virtual int Execute() override final;
  virtual std::string GetUsage() const override final;
  virtual bool AddSubCommands() override final { return true; }
};


}  // namespace wwivutil
}  // namespace wwiv

#endif  // __INCLUDED_WWIVUTIL_DUMP_CALLOUT_H__
