#include "Utils.h"

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/timeb.h>
#include <iconv.h>

#include <sstream>

using namespace std;

namespace loong
{
	int getMsecTime(char *strTm)
	{
		struct timeb tp;
		struct tm *tm;

		ftime(&tp);
		tm = localtime(&(tp.time));

		snprintf(strTm, 26, "%04d-%02d-%02d %02d:%02d:%02d:%03d",
			1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec, tp.millitm);

		return 0;
	}

	void getCurrDateTime(char *datetime)
	{
		time_t tick;
		struct tm tm;

		tick = time(nullptr);
		tm = *localtime(&tick);
		strftime(datetime, 90, "%Y-%m-%d %H:%M:%S", &tm);
	}

	void getCurrDate(char *currDate)
	{
		time_t tick;
		struct tm tm;
		tick = time(nullptr);
		tm = *localtime(&tick);
		strftime(currDate, 9, "%Y%m%d", &tm);
	}

	long datetimeToTimestamp(const char *datetime)
	{
		char fmt[] = "%Y-%m-%d %H:%M:%S";
		struct tm tb;
		if (strptime(datetime, fmt, &tb) != nullptr)
		{
			return mktime(&tb);
		}
		else
		{
			return -1;
		}
	}

	uint64_t ntohll(uint64_t val)
	{
		if (__BYTE_ORDER == __LITTLE_ENDIAN)
		{
			return swap64BtyeOrder(val);
		}
		else if (__BYTE_ORDER == __BIG_ENDIAN)
		{
			return val;
		}
	}

	uint64_t htonll(uint64_t val)
	{
		if (__BYTE_ORDER == __LITTLE_ENDIAN)
		{
			return swap64BtyeOrder(val);
		}
		else if (__BYTE_ORDER == __BIG_ENDIAN)
		{
			return val;
		}
	}

	int movefile(const char *src_path, char *dst_path)
	{
		char cmd[256] = { '0' };
		snprintf(cmd, sizeof(cmd), "mv %s %s", src_path, dst_path);
		//检查要移动的文件是否存在
		if (access(src_path, F_OK) < 0)
		{
			printf("%s[%d]\n", strerror(errno), __LINE__);
			return -1;
		}

		//检查目标路径是否存在，不存在则创建
		if (createdir(dst_path) < 0)
		{
			return -1;
		}

		system(cmd);
		return 0;
	}

	/*检查目标路径是否存在，不存在则创建*/
	int createdir(const char *path)
	{
		struct stat buf;
		stat(path, &buf);
		if (!S_ISDIR(buf.st_mode))
		{
			printf("%s dir not exist! will create %s[%d]\n", path, strerror(errno), __LINE__);
			if (mkdir(path, 0755) < 0)
			{
				printf("create dir %s faile: %s[%d]\n", path, strerror(errno), __LINE__);
				return -1;
			}
		}
		return 0;
	}

	enum { LEFT_TRIM = 1, RIGHT_TRIM = 2, MIDDLE_TRIM = 8 };
	char *strtrim(char* str, const char* delim, int trimtype)
	{
		char* p1 = 0;
		char* p2 = 0;
		int len = strlen(str);

		if (str == 0 || str[0] == 0) return str;

		if (trimtype & LEFT_TRIM) {
			p1 = str;
			while (*p1 && strchr(delim, *p1))
				p1++;

			if (str != p1) {
				strcpy(str, p1);
				if (*str == 0) return str;
				len -= (p1 - str);
			}
		}

		if (trimtype & RIGHT_TRIM) {
			p1 = str;
			p2 = p1 + len - 1;
			while (p2 > p1 && strchr(delim, *p2))
				p2--;


			if (p2 < p1 + len - 1) {
				*(p2 + 1) = '\0';
				if (*str == 0) return str;
				len -= (p1 + len - 1 - p2);
			}
		}
		if (trimtype & MIDDLE_TRIM) {
			int i, j = 0;
			p1 = (char*)alloca(len + 1);
			for (i = 0; i < len; i++) {
				if (!strchr(delim, str[i]))
					p1[j++] = str[i];
			}
			if (j != len) {
				p1[j] = '\0';
				strcpy(str, p1);
			}
		}
		return str;
	}

	//must init *pcount=sizeof(args)/0 first
	//aaa=bbb&ccc=ddd&eee=fff
	int parsePairsString(char *src, const char *split_sec,
		const char *split_key, const char *trim_str, char **args, int *pcount)
	{
		int slen, klen;
		char *p1, *p2;
		int count = 0;

		slen = strlen(split_sec);
		klen = strlen(split_key);
		//tlen = strlen (trim_str);
		do {
			if (count + 2 >= *pcount)
				break;
			while (*src && strchr(trim_str, *src)) {
				src++;
			}
			if (!*src)
				break;
			p1 = strstr(src, split_sec);
			if (p1) {
				*p1 = '\0';
				if (p1 == src) {
					break;
				}
				p1 += slen;
			}

			p2 = strstr(src, split_key);
			if (p2) {
				*p2 = '\0';
				p2 += klen;
				while (*p2 && strchr(trim_str, *p2)) {
					p2++;
				}
				strtrim(p2, trim_str, RIGHT_TRIM);
			}
			else {
				p2 = (char*)"";
			}
			strtrim(src, trim_str, RIGHT_TRIM);
			if (!*src) {
				break;
			}
			args[count++] = src;
			args[count++] = p2;

			//next section
			src = p1;
		} while (src && *src);

		*pcount = count;
		return 0;
	}

	//解析逗号分隔的字串
	vector<string> parseDotString(string inp, string dot)
	{
		vector<string> oup;
		oup.reserve(1024);
		size_t pois = 0, ppois = 0, fpois = 0;
		inp += dot;
		while ((pois = inp.find(dot, ppois)) != string::npos)
		{
			string tmp_str = inp.substr(ppois, pois - ppois);
			if ((fpois = tmp_str.find_first_not_of(" \t\n")) != string::npos)
			{
				tmp_str = tmp_str.substr(fpois);
			}
			if ((fpois = tmp_str.find_first_of(" \t\n")) != string::npos)
			{
				tmp_str = tmp_str.substr(0, fpois);
			}
			oup.push_back(tmp_str);
			ppois = pois + dot.length();
		}
		return oup;
	}

	void codeConvert(char * from, char * to, char * src, char * dst, size_t dstSize)
	{
		size_t sl, dl;
		sl = strlen(src);
		dl = dstSize;
		char * pIn = src;
		char * pOut = dst;
		memset(dst, '\0', dstSize);
		iconv_t conv = iconv_open(to, from);
		iconv(conv, &pIn, &sl, &pOut, &dl);
		iconv_close(conv);
	}

	void gbk2utf8(char * src, char * dst, size_t dstSize)
	{
		char * gbk = (char*)"GBK";
		char * utf8 = (char*)"UTF-8";
		codeConvert(gbk, utf8, src, dst, dstSize);
	}

	bool isIp(const char *ipstr)
	{
		int num;
		int dots = 0;
		char *ptr;
		const char *DELIM = ".";

		if (ipstr == nullptr)
		{
			return false;
		}
		char pIpStr[20] = { '\0' };
		strncpy(pIpStr, ipstr, 20);

		ptr = strtok(pIpStr, DELIM);
		if (ptr == nullptr)
		{
			return false;
		}

		while (ptr)
		{
			char *tmp = ptr;
			while (*tmp != '\0')
			{
				if (!isdigit(*tmp))
				{
					return false;
				}
				++tmp;
			}

			num = atoi(ptr);
			if (num >= 0 && num <= 255)
			{
				ptr = strtok(nullptr, DELIM);
				if (ptr != nullptr)
				{
					++dots;
				}
			}
			else
			{
				return false;
			}
		}

		if (dots != 3)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
