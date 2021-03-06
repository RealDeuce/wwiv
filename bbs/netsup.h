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
#ifndef __INCLUDED_BBS_NETSUP_H__
#define __INCLUDED_BBS_NETSUP_H__

#include "sdk/net.h"

void cleanup_net();
int  cleanup_net1();
void do_callout(int sn);
void attempt_callout();
void print_pending_list();
void gate_msg(net_header_rec * nh, char *messageText, int nNetNumber, const char *pszAuthorName,
  unsigned short int *pList, int nFromNetworkNumber);
void force_callout(int dw);
long *next_system_reg(int ts);
void run_exp();



#endif  // __INCLUDED_BBS_NETSUP_H__