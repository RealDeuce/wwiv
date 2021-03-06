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
#include <memory>
#include <string>

#include "bbs/datetime.h"
#include "bbs/bbs.h"
#include "bbs/fcns.h"
#include "bbs/vars.h"
#include "bbs/wstatus.h"
#include "core/datafile.h"
#include "core/file.h"
#include "core/wwivassert.h"
#include "sdk/filenames.h"

statusrec status;

using std::string;
using std::unique_ptr;
using namespace wwiv::core;

// WStatus
const int WStatus::fileChangeNames = 0;
const int WStatus::fileChangeUpload = 1;
const int WStatus::fileChangePosts = 2;
const int WStatus::fileChangeEmail = 3;
const int WStatus::fileChangeNet = 4;

const char* WStatus::GetLastDate(int nDaysAgo) const {
  WWIV_ASSERT(nDaysAgo >= 0);
  WWIV_ASSERT(nDaysAgo <= 2);
  switch (nDaysAgo) {
  case 0:
    return m_pStatusRecord->date1;
  case 1:
    return m_pStatusRecord->date2;
  case 2:
    return m_pStatusRecord->date3;
  default:
    return m_pStatusRecord->date1;
  }
}

const char* WStatus::GetLogFileName(int nDaysAgo) const {
  WWIV_ASSERT(nDaysAgo >= 0);
  switch (nDaysAgo) {
  case 0: {
    static char s[81]; // logname
    std::string todays_log = GetSysopLogFileName(date());
    strcpy(s, todays_log.c_str());
    return s;
  }
  case 1:
    return m_pStatusRecord->log1;
  case 2:
    return m_pStatusRecord->log2;
  default:
    return m_pStatusRecord->log1;
  }
}

void WStatus::EnsureCallerNumberIsValid() {
  if (m_pStatusRecord->callernum != 65535) {
    this->SetCallerNumber(m_pStatusRecord->callernum);
    m_pStatusRecord->callernum = 65535;
  }
}

void WStatus::ValidateAndFixDates() {
  if (m_pStatusRecord->date1[8] != '\0') {
    m_pStatusRecord->date1[6] = '0';
    m_pStatusRecord->date1[7] = '0';
    m_pStatusRecord->date1[8] = '\0'; // forgot to add null termination
  }

  string currentDate = date();
  if (m_pStatusRecord->date3[8] != '\0') {
    m_pStatusRecord->date3[6] = currentDate[6];
    m_pStatusRecord->date3[7] = currentDate[7];
    m_pStatusRecord->date3[8] = '\0';
  }
  if (m_pStatusRecord->date2[8] != '\0') {
    m_pStatusRecord->date2[6] = currentDate[6];
    m_pStatusRecord->date2[7] = currentDate[7];
    m_pStatusRecord->date2[8] = '\0';
  }
  if (m_pStatusRecord->date1[8] != '\0') {
    m_pStatusRecord->date1[6] = currentDate[6];
    m_pStatusRecord->date1[7] = currentDate[7];
    m_pStatusRecord->date1[8] = '\0';
  }
  if (m_pStatusRecord->gfiledate[8] != '\0') {
    m_pStatusRecord->gfiledate[6] = currentDate[6];
    m_pStatusRecord->gfiledate[7] = currentDate[7];
    m_pStatusRecord->gfiledate[8] = '\0';
  }
}

bool WStatus::NewDay() {
  m_pStatusRecord->callstoday   = 0;
  m_pStatusRecord->msgposttoday = 0;
  m_pStatusRecord->localposts   = 0;
  m_pStatusRecord->emailtoday   = 0;
  m_pStatusRecord->fbacktoday   = 0;
  m_pStatusRecord->uptoday      = 0;
  m_pStatusRecord->activetoday  = 0;
  m_pStatusRecord->days++;

  // Need to verify the dates aren't trashed otherwise we can crash here.
  ValidateAndFixDates();

  strcpy(m_pStatusRecord->date3, m_pStatusRecord->date2);
  strcpy(m_pStatusRecord->date2, m_pStatusRecord->date1);
  strcpy(m_pStatusRecord->date1, date());
  strcpy(m_pStatusRecord->log2, m_pStatusRecord->log1);

  const string log = GetSysopLogFileName(GetLastDate(1));
  strcpy(m_pStatusRecord->log1, log.c_str());
  return true;
}

// StatusMgr
bool StatusMgr::Get(bool bLockFile) {
  if (!m_statusFile.IsOpen()) {
    m_statusFile.SetName(session()->config()->datadir(), STATUS_DAT);
    int nLockMode = (bLockFile) ? (File::modeReadWrite | File::modeBinary) : (File::modeReadOnly | File::modeBinary);
    m_statusFile.Open(nLockMode);
  } else {
    m_statusFile.Seek(0L, File::seekBegin);
  }
  if (!m_statusFile.IsOpen()) {
    sysoplog("CANNOT READ STATUS");
    return false;
  } else {
    char oldFileChangeFlags[7];
    for (int nFcIndex = 0; nFcIndex < 7; nFcIndex++) {
      oldFileChangeFlags[nFcIndex] = status.filechange[nFcIndex];
    }
    m_statusFile.Read(&status, sizeof(statusrec));

    if (!bLockFile) {
      m_statusFile.Close();
    }

    for (int i = 0; i < 7; i++) {
      if (oldFileChangeFlags[i] != status.filechange[i]) {
        switch (i) {
        case WStatus::fileChangeNames: {        
          // re-read names.lst
          if (session()->names()) {
            // We may not have the BBS initialized yet, so only
            // re-read the names file if it's changed from another node.
            session()->names()->Load();
          }
        } break;
        case WStatus::fileChangeUpload:
        break;
        case WStatus::fileChangePosts:
          session()->subchg = 1;
          break;
        case WStatus::fileChangeEmail:
          emchg = true;
          mailcheck = false;
          break;
        case WStatus::fileChangeNet: {
          int nOldNetNum = session()->net_num();
          zap_bbs_list();
          for (int i1 = 0; i1 < session()->max_net_num(); i1++) {
            set_net_num(i1);
            zap_call_out_list();
            zap_contacts();
          }
          set_net_num(nOldNetNum);
        }
        break;
        }
      }
    }
  }
  return true;
}

bool StatusMgr::RefreshStatusCache() {
  return this->Get(false);
}

WStatus* StatusMgr::GetStatus() {
  this->Get(false);
  return new WStatus(&status);
}

void StatusMgr::AbortTransaction(WStatus* pStatus) {
  unique_ptr<WStatus> deleter(pStatus);
  if (m_statusFile.IsOpen()) {
    m_statusFile.Close();
  }
}

WStatus* StatusMgr::BeginTransaction() {
  this->Get(true);
  return new WStatus(&status);
}

bool StatusMgr::CommitTransaction(WStatus* pStatus) {
  unique_ptr<WStatus> deleter(pStatus);
  return this->Write(pStatus->m_pStatusRecord);
}

bool StatusMgr::Write(statusrec *pStatus) {
  if (!m_statusFile.IsOpen()) {
    m_statusFile.SetName(session()->config()->datadir(), STATUS_DAT);
    m_statusFile.Open(File::modeReadWrite | File::modeBinary);
  } else {
    m_statusFile.Seek(0L, File::seekBegin);
  }

  if (!m_statusFile.IsOpen()) {
    sysoplog("CANNOT SAVE STATUS");
    return false;
  }
  m_statusFile.Write(pStatus, sizeof(statusrec));
  m_statusFile.Close();
  return true;
}

const int StatusMgr::GetUserCount() {
  unique_ptr<WStatus>pStatus(GetStatus());
  return pStatus->GetNumUsers();
}
