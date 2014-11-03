/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
/*             Copyright (C)1998-2014, WWIV Software Services             */
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

#include "wwiv.h"
#include "core/wutil.h"
#include "bbs/wcomm.h"
#include "core/strings.h"
#include "core/wtextfile.h"
#include "core/wwivassert.h"
#include "core/wwivport.h"

using std::string;
using std::unique_ptr;
using wwiv::strings::StrCat;
using wwiv::strings::StringPrintf;

/**
 * Returns true if local sysop functions accessible, else returns false.
 */
bool AllowLocalSysop() {
  return (syscfg.sysconfig & sysconfig_no_local) ? false : true;
}


/**
 * Finds GetSession()->usernum and system number from emailAddress, sets network number as
 * appropriate.
 * @param emailAddress The text of the email address.
 * @param pUserNumber OUT The User Number
 * @param pSystemmNumber OUT The System Number
 */
void parse_email_info(const string& emailAddress, int *pUserNumber, int *pSystemNumber) {
  char *ss1, onx[20], ch, *mmk;
  unsigned nUserNumber, nSystemNumber;
  int i, nv, on, xx, onxi, odci;
  net_system_list_rec *csne;

  char szEmailAddress[ 255 ];
  strcpy(szEmailAddress, emailAddress.c_str());

  *pUserNumber = 0;
  *pSystemNumber = 0;
  net_email_name[0] = '\0';
  char *ss = strrchr(szEmailAddress, '@');
  if (ss == nullptr) {
    nUserNumber = finduser1(szEmailAddress);
    if (nUserNumber > 0) {
      *pUserNumber = static_cast< unsigned short >(nUserNumber);
    } else if (wwiv::strings::IsEquals(szEmailAddress, "SYSOP")) {     // Add 4.31 Build3
      *pUserNumber = 1;
    } else {
      bout << "Unknown user.\r\n";
    }
  } else if (atoi(ss + 1) == 0) {
    for (i = 0; i < GetSession()->GetMaxNetworkNumber(); i++) {
      set_net_num(i);
      if ((strncasecmp("internet", GetSession()->GetNetworkName(), 8) == 0) ||
          ((strncasecmp("filenet", GetSession()->GetNetworkName(), 7) == 0) && (*pSystemNumber == 32767))) {
        strcpy(net_email_name, szEmailAddress);
        for (ss1 = net_email_name; *ss1; ss1++) {
          if ((*ss1 >= 'A') && (*ss1 <= 'Z')) {
            *ss1 += 'a' - 'A';
          }
        }
        *pSystemNumber = 1;
        break;
      }
    }
    if (i >= GetSession()->GetMaxNetworkNumber()) {
      bout << "Unknown user.\r\n";
    }
  } else {
    ss[0] = '\0';
    ss = &(ss[1]);
    i = strlen(szEmailAddress);
    while (i > 0 && szEmailAddress[i - 1] == ' ') {
      --i;
    }
    szEmailAddress[i] = 0;
    nUserNumber = atoi(szEmailAddress);
    if (nUserNumber == 0 && szEmailAddress[0] == '#') {
      nUserNumber = atoi(szEmailAddress + 1);
    }
    if (strchr(szEmailAddress, '@')) {
      nUserNumber = 0;
    }
    nSystemNumber = atoi(ss);
    ss1 = strchr(ss, '.');
    if (ss1) {
      ss1++;
    }
    if (nUserNumber == 0) {
      strcpy(net_email_name, szEmailAddress);
      i = strlen(net_email_name);
      while (i > 0 && net_email_name[i - 1] == ' ') {
        --i;
      }
      net_email_name[i] = '\0';
      if (net_email_name[0]) {
        *pSystemNumber = static_cast< unsigned short >(nSystemNumber);
      } else {
        bout << "Unknown user.\r\n";
      }
    } else {
      *pUserNumber = static_cast< unsigned short >(nUserNumber);
      *pSystemNumber = static_cast< unsigned short >(nSystemNumber);
    }
    if (*pSystemNumber && ss1) {
      for (i = 0; i < GetSession()->GetMaxNetworkNumber(); i++) {
        set_net_num(i);
        if (wwiv::strings::IsEqualsIgnoreCase(ss1, GetSession()->GetNetworkName())) {
          if (!valid_system(*pSystemNumber)) {
            bout.nl();
            bout << "There is no " << ss1 << " @" << *pSystemNumber << ".\r\n\n";
            *pSystemNumber = *pUserNumber = 0;
          } else {
            if (*pSystemNumber == net_sysnum) {
              *pSystemNumber = 0;
              if (*pUserNumber == 0) {
                *pUserNumber = static_cast< unsigned short >(finduser(net_email_name));
              }
              if (*pUserNumber == 0 || *pUserNumber > 32767) {
                *pUserNumber = 0;
                bout << "Unknown user.\r\n";
              }
            }
          }
          break;
        }
      }
      if (i >= GetSession()->GetMaxNetworkNumber()) {
        bout.nl();
        bout << "This system isn't connected to " << ss1 << "\r\n";
        *pSystemNumber = *pUserNumber = 0;
      }
    } else if (*pSystemNumber && GetSession()->GetMaxNetworkNumber() > 1) {
      odc[0] = '\0';
      odci = 0;
      onx[0] = 'Q';
      onx[1] = '\0';
      onxi = 1;
      nv = 0;
      on = GetSession()->GetNetworkNumber();
      ss = static_cast<char *>(BbsAllocA(GetSession()->GetMaxNetworkNumber()));
      WWIV_ASSERT(ss != nullptr);
      xx = -1;
      for (i = 0; i < GetSession()->GetMaxNetworkNumber(); i++) {
        set_net_num(i);
        if (net_sysnum == *pSystemNumber) {
          xx = i;
        } else if (valid_system(*pSystemNumber)) {
          ss[nv++] = static_cast< char >(i);
        }
      }
      set_net_num(on);
      if (nv == 0) {
        if (xx != -1) {
          set_net_num(xx);
          *pSystemNumber = 0;
          if (*pUserNumber == 0) {
            *pUserNumber = static_cast< unsigned short >(finduser(net_email_name));
            if (*pUserNumber == 0 || *pUserNumber > 32767) {
              *pUserNumber = 0;
              bout << "Unknown user.\r\n";
            }
          }
        } else {
          bout.nl();
          bout << "Unknown system\r\n";
          *pSystemNumber = *pUserNumber = 0;
        }
      } else if (nv == 1) {
        set_net_num(ss[0]);
      } else {
        bout.nl();
        for (i = 0; i < nv; i++) {
          set_net_num(ss[i]);
          csne = next_system(*pSystemNumber);
          if (csne) {
            if (i < 9) {
              onx[onxi++] = static_cast< char >(i + '1');
              onx[onxi] = 0;
            } else {
              odci = static_cast< char >((i + 1) / 10);
              odc[odci - 1] = static_cast< char >(odci + '0');
              odc[odci] = 0;
            }
            bout << i + 1 << ". " << GetSession()->GetNetworkName() << " (" << csne->name << ")\r\n";
          }
        }
        bout << "Q. Quit\r\n\n";
        bout << "|#2Which network (number): ";
        if (nv < 9) {
          ch = onek(onx);
          if (ch == 'Q') {
            i = -1;
          } else {
            i = ch - '1';
          }
        } else {
          mmk = mmkey(2);
          if (*mmk == 'Q') {
            i = -1;
          } else {
            i = atoi(mmk) - 1;
          }
        }
        if (i >= 0 && i < nv) {
          set_net_num(ss[i]);
        } else {
          bout << "\r\n|#6Aborted.\r\n\n";
          *pUserNumber = *pSystemNumber = 0;
        }
      }
      free(ss);
    } else {
      if (*pSystemNumber == net_sysnum) {
        *pSystemNumber = 0;
        if (*pUserNumber == 0) {
          *pUserNumber = static_cast< unsigned short >(finduser(net_email_name));
        }
        if (*pUserNumber == 0 || *pUserNumber > 32767) {
          *pUserNumber = 0;
          bout << "Unknown user.\r\n";
        }
      } else if (!valid_system(*pSystemNumber)) {
        bout << "\r\nUnknown user.\r\n";
        *pSystemNumber = *pUserNumber = 0;
      }
    }
  }
}


/**
 * Queries user and verifies system password.
 * @return true if the password entered is valid.
 */
bool ValidateSysopPassword() {
  bout.nl();
  if (so()) {
    if (incom) {
      string password;
      input_password("|#7SY: ", &password, 20);
      if (password == syscfg.systempw) {
        return true;
      }
    } else if (AllowLocalSysop()) {
      return true;
    }
  }
  return false;
}

/**
 * Hangs up the modem if user online. Whether using modem or not, sets
 * hangup to 1.
 */
void hang_it_up() {
  hangup = true;
#if !defined( __unix__ )
  if (!ok_modem_stuff) {
    return;
  }

  GetSession()->remoteIO()->dtr(false);
  if (!GetSession()->remoteIO()->carrier()) {
    return;
  }

  Wait(0.5);
  if (!GetSession()->remoteIO()->carrier()) {
    return;
  }

  Wait(0.5);
  if (!GetSession()->remoteIO()->carrier()) {
    return;
  }
#endif
}

/**
 * Plays a sound definition file (*.sdf) through PC speaker. SDF files
 * should reside in the gfiles dir. The only params passed to function are
 * filename and false if playback is unabortable, true if it is abortable. If no
 * extension then .SDF is appended. A full path to file may be specified to
 * override gfiles dir. Format of file is:
 *
 * <freq> <duration in ms> [pause_delay in ms]
 * 1000 1000 50
 *
 * Returns 1 if sucessful, else returns 0. The pause_delay is optional and
 * is used to insert silences between tones.
 */
bool play_sdf(const string& soundFileName, bool abortable) {
  WWIV_ASSERT(!soundFileName.empty());

  string fullPathName;
  // append gfilesdir if no path specified
  if (soundFileName.find(WFile::pathSeparatorChar) == string::npos) {
    fullPathName = StrCat(syscfg.gfilesdir, soundFileName);
  } else {
    fullPathName = soundFileName;
  }

  // append .SDF if no extension specified
  if (fullPathName.find('.') == string::npos) {
    fullPathName += ".sdf";
  }

  // Must Exist
  if (!WFile::Exists(fullPathName)) {
    return false;
  }

  // must be able to open read-only
  WTextFile soundFile(fullPathName, "rt");
  if (!soundFile.IsOpen()) {
    return false;
  }

  // scan each line, ignore lines with words<2
  string soundLine;
  while (soundFile.ReadLine(&soundLine)) {
    if (abortable && bkbhit()) {
      break;
    }
    int nw = wordcount(soundLine.c_str(), DELIMS_WHITE);
    if (nw >= 2) {
      int freq = atoi(extractword(1, soundLine, DELIMS_WHITE));
      int dur = atoi(extractword(2, soundLine, DELIMS_WHITE));

      // only play if freq and duration > 0
      if (freq > 0 && dur > 0) {
        int nPauseDelay = 0;
        if (nw > 2) {
          nPauseDelay = atoi(extractword(3, soundLine, DELIMS_WHITE));
        }
        WWIV_Sound(freq, dur);
        if (nPauseDelay > 0) {
          WWIV_Delay(nPauseDelay);
        }
      }
    }
  }

  soundFile.Close();
  return true;
}


/**
 * Describes the area code as listed in regions.dat
 * @param nAreaCode The area code to describe
 * @param pszDescription point to return the description for the specified
 *        area code.
 */
string describe_area_code(int nAreaCode) {
  WTextFile file(syscfg.datadir, REGIONS_DAT, "rt");
  if (!file.IsOpen()) {
    // Failed to open regions area code file
    return "";
  }

  string previous;
  string current;
  while (file.ReadLine(&current)) {
    int nCurrentTown = atoi(current.c_str()); 
    if (nCurrentTown == nAreaCode) {
      return previous;
    } else if (nCurrentTown == 0) {
      // Only set this on values that are town names and not area codes.
      previous = current;
    }
  }
  return "";
}

/**
 * Describes the Town (area code + prefix) as listed in the regions file.
 * @param nAreaCode The area code to describe
 * @param nTargetTown The phone number prefix to describe
 * @return the description for the specified area code.
 */
string describe_area_code_prefix(int nAreaCode, int nTargetTown) {
  const string filename = StringPrintf("%s%s%c%s.%-3d",
          syscfg.datadir,
          REGIONS_DIR,
          WFile::pathSeparatorChar,
          REGIONS_DIR,
          nAreaCode);
  WTextFile file(filename, "rt");
  if (!file.IsOpen()) {
    // Failed to open regions area code file
    return "";
  }

  string previous;
  string current;
  while (file.ReadLine(&current)) {
    int nCurrentTown = atoi(current.c_str()); 
    if (nCurrentTown == nTargetTown) {
      return previous;
    } else if (nCurrentTown == 0) {
      // Only set this on values that are town names and not area codes.
      previous = current;
    }
  }
  return "";
}
