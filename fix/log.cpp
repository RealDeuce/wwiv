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
#include "fix/log.h"

#include <cstdarg>
#include <ctime>

#include "core/file.h"
#include "core/version.h"

static File hLogFile;

static const char* szLogTypeArray[] = {
	"+ ",
	"! ",
	"? ",
	0
};

void Print(int nType, bool bLogIt, const char* szText, ...) {
	char buffer[256];
	va_list ap;

	va_start(ap, szText);
	vsprintf(buffer, szText, ap);
	va_end(ap);
	printf("%s%s\n", szLogTypeArray[nType], buffer);

	if (bLogIt && hLogFile.IsOpen()) {
    char str[81];
    char szBuf[512];
    time_t secs_now = time(nullptr);
    struct tm* time_now = localtime(&secs_now);
		strftime(str, 80, "%H:%M:%S", time_now);
		sprintf(szBuf, "%s%s  %s\n", szLogTypeArray[nType], str, buffer);
		hLogFile.Write(szBuf, strlen(szBuf));
	}
}

bool OpenLogFile(const char* szFileName) {
	struct tm *time_now;
	time_t secs_now;
	char str[81];
	char szBuf[512];

	hLogFile.SetName(szFileName);
	if (!hLogFile.Open(File::modeReadWrite | File::modeCreateFile)) {
		Print(NOK, false, "Cannot open Log File %s", szFileName);
		return false;
	}

	tzset();
	time(&secs_now);
	time_now = localtime(&secs_now);
	strftime(str, 80, "%a %d %b %Y", time_now);
	sprintf(szBuf, "\n----------  %s, fix %s%s\n", str, wwiv_version, beta_version);
	hLogFile.Write(szBuf, strlen(szBuf));

	return true;
}


void CloseLogFile() {
	if (hLogFile.IsOpen()) {
		hLogFile.Close();
	}
}
