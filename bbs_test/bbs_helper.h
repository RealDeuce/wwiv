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
/*                                                                        */
/**************************************************************************/
#ifndef __INCLUDED_BBS_HELPER_H__
#define __INCLUDED_BBS_HELPER_H__

#include <memory>
#include <string>
#include "bbs/bbs.h"
#include "core_test/file_helper.h"
#include "bbs/local_io.h"
#include "sdk/user.h"

class TestIO;

/*
 * 
 * Test helper for using code with heavy BBS dependencies.
 * 
 * To use, add an instance as a field in the test class.  Then invoke.
 * BbsHelper::SetUp before use. typically from Test::SetUp.
 */
class BbsHelper {
public:
    virtual void SetUp();
    virtual void TearDown();
    wwiv::sdk::User* user() const { return user_; }
    TestIO* io() const { return io_.get(); }

    // Accessors for various directories
    FileHelper& files() { return files_; }
    const std::string& data() { return dir_data_; }
    const std::string& gfiles() { return dir_gfiles_; }

public:
    FileHelper files_;
    std::string dir_data_;
    std::string dir_gfiles_;
    std::string dir_en_gfiles_;
    std::string dir_menus_;
    std::unique_ptr<WApplication> app_;
    std::unique_ptr<WSession> sess_;
    std::unique_ptr<TestIO> io_;
    wwiv::sdk::User* user_;
};

class TestIO {
public:
  TestIO();
  void Clear() { captured_.clear(); } 
  std::string captured();
  LocalIO* local_io() const { return local_io_; }
private:
  LocalIO* local_io_;
  std::string captured_;
};

class TestLocalIO : public LocalIO {
public:
  TestLocalIO(std::string* captured);
  virtual void LocalPutch(unsigned char ch) override;
  virtual void LocalGotoXY(int x, int y) override {}
  virtual size_t WhereX() override { return 0; }
  virtual size_t WhereY() override { return 0; }
  virtual void LocalLf() override {}
  virtual void LocalCr() override {}
  virtual void LocalCls() override {}
  virtual void LocalBackspace() override {}
  virtual void LocalPutchRaw(unsigned char ch) override {}
  virtual void LocalPuts(const std::string& s) override {}
  virtual void LocalXYPuts(int x, int y, const std::string& text) override {}
  virtual void LocalFastPuts(const std::string& text) override {}
  virtual int LocalPrintf(const char *formatted_text, ...) override { return 0; }
  virtual int LocalXYPrintf(int x, int y, const char *formatted_text, ...) override { return 0; }
  virtual int LocalXYAPrintf(int x, int y, int nAttribute, const char *formatted_text, ...) override { return 0; }
  virtual void set_protect(WSession* session, int l) override {}
  virtual void savescreen() override {}
  virtual void restorescreen() override {}
  virtual void tleft(WSession* session, bool temp_sysop, bool sysop, bool user_online) override {}
  virtual bool LocalKeyPressed() override { return false; }
  virtual void SaveCurrentLine(char *cl, char *atr, char *xl, char *cc) override {}
  virtual unsigned char LocalGetChar() override { return getchar(); }
  virtual void MakeLocalWindow(int x, int y, int xlen, int ylen) override {}
  virtual void SetCursor(int cursorStyle) override {}
  virtual void LocalClrEol() override {}
  virtual void LocalWriteScreenBuffer(const char *buffer) override {}
  virtual size_t GetDefaultScreenBottom() override { return 25; }
  virtual void LocalEditLine(char *s, int len, int status, int *returncode, char *ss) override {}
  virtual void UpdateNativeTitleBar(WSession* session) override {}
  virtual void UpdateTopScreen(WStatus* pStatus, WSession *pSession, int nInstanceNumber) override {}

  std::string* captured_;
};

#endif // __INCLUDED_BBS_HELPER_H__
