/**************************************************************************/
/*                                                                        */
/*                  WWIV Initialization Utility Version 5                 */
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
#include <cstdlib>
#include <string>
#include <vector>

#include "init/wwivinit.h"
#include "core/file.h"
#include "core/stl.h"
#include "core/strings.h"
#include "sdk/vardec.h"
#include "init/subacc.h"


using std::vector;
using namespace wwiv::stl;
using namespace wwiv::strings;

static File fileSub;                       // File object for '.sub' file
static char subdat_fn[MAX_PATH];            // filename of .sub file

// locals
static int m_nCurrentReadMessageArea, subchg;
static int  GetCurrentReadMessageArea() {
  return m_nCurrentReadMessageArea;
}

static void SetCurrentReadMessageArea(int n) {
  m_nCurrentReadMessageArea = n;
}

static int nNumMsgsInCurrentSub;

int GetNumMessagesInCurrentMessageArea() {
  return nNumMsgsInCurrentSub;
}

static void SetNumMessagesInCurrentMessageArea(int n) {
  nNumMsgsInCurrentSub = n;
}

void close_sub() {
  if (fileSub.IsOpen()) {
    fileSub.Close();
  }
}

bool open_sub(bool wr) {
  postrec p{};

  close_sub();

  if (wr) {
    fileSub.SetName(subdat_fn);
    fileSub.Open(File::modeBinary | File::modeCreateFile | File::modeReadWrite);

    if (fileSub.IsOpen()) {
      // re-read info from file, to be safe
      fileSub.Seek(0L, File::seekBegin);
      fileSub.Read(&p, sizeof(postrec));
      SetNumMessagesInCurrentMessageArea(p.owneruser);
    }
  } else {
    fileSub.SetName(subdat_fn);
    fileSub.Open(File::modeReadOnly | File::modeBinary);
  }

  return fileSub.IsOpen();
}

bool iscan1(int si, const vector<subboardrec>& subboards) {
  // Initializes use of a sub value (subboards[], not usub[]).  If quick, then
  // don't worry about anything detailed, just grab qscan info.
  postrec p{};

  // forget it if an invalid sub #
  if (si < 0 || si >= size_int(subboards)) {
    return false;
  }

  // see if a sub has changed
  if (subchg) {
    SetCurrentReadMessageArea(-1);
  }

  // if already have this one set, nothing more to do
  if (si == GetCurrentReadMessageArea()) {
    return true;
  }

  // set sub filename
  snprintf(subdat_fn, sizeof(subdat_fn), "%s%s.sub", syscfg.datadir, subboards[si].filename);

  // open file, and create it if necessary
  if (!File::Exists(subdat_fn)) {
    if (!open_sub(true)) {
      return false;
    }
    p.owneruser = 0;
    fileSub.Write(&p, sizeof(postrec));
  } else if (!open_sub(false)) {
    return false;
  }

  // set sub
  SetCurrentReadMessageArea(si);
  subchg = 0;

  // read in first rec, specifying # posts
  fileSub.Seek(0L, File::seekBegin);
  fileSub.Read(&p, sizeof(postrec));
  SetNumMessagesInCurrentMessageArea(p.owneruser);

  // close file
  close_sub();

  // iscanned correctly
  return true;
}

postrec *get_post(int mn) {
  // Returns info for a post.  Maintains a cache.  Does not correct anything
  // if the sub has changed.
  static postrec p{};
  // error if msg # invalid
  if (mn < 1) {
    return nullptr;
  }
  // adjust msgnum, if it is no longer valid
  if (mn > GetNumMessagesInCurrentMessageArea()) {
    mn = GetNumMessagesInCurrentMessageArea();
  }

  // read in some sub info
  fileSub.Seek(mn * sizeof(postrec), File::seekBegin);
  fileSub.Read(&p, sizeof(postrec));
  return &p;
}

void write_post(int mn, postrec * pp) {
  if (fileSub.IsOpen()) {
    fileSub.Seek(mn * sizeof(postrec), File::seekBegin);
    fileSub.Write(pp, sizeof(postrec));
  }
}
