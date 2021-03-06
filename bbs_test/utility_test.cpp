/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*           Copyright (C)2014-2016 WWIV Software Services                */
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
/*                                                                        */
/**************************************************************************/

#include <ctime>
#include <string>
#include "gtest/gtest.h"
#include "bbs_test/bbs_helper.h"

#include "bbs/utility.h"

static const time_t MAY_18_2014 = 1400461200;

using std::cout;
using std::endl;
using std::string;

class UtilityTest : public ::testing::Test {
protected:
    virtual void SetUp() {
      helper.SetUp();
    }
    BbsHelper helper;
};

TEST_F(UtilityTest, DateString_Y) {
    string res = W_DateString(MAY_18_2014, "Y", "");
    ASSERT_EQ("2014", res);

    string res2 = W_DateString(MAY_18_2014, "YY", "");
    ASSERT_EQ("2014 2014", res2);
}

// TODO(rushfan): Fix this dealing with PDT vs. GMT build machine
#if 0
TEST_F(UtilityTest, DateString_WDT) {
    string res = W_DateString(MAY_18_2014, "WDT", "");
    ASSERT_EQ("Sunday, May 18, 2014 06:00 PM", res);

    string res2 = W_DateString(MAY_18_2014, "WDT", "at");
    ASSERT_EQ("Sunday, May 18, 2014 at 06:00 PM", res2);
}
#endif  // 0
