#include "IniFile.h"

#include <string.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace loong
{
	const int BUF_SIZE = 1024;
	const int ITEM_SIZE = 128;

	bool IniFile::init(const char *fileName)
	{
		ifstream fin(fileName, ios::in | ios::binary);
		string tmp;
		if (!fin.is_open())
		{
			cout << "open ini file failed!" << endl;
			return false;
		}
		char buff[BUF_SIZE] = { '\0' };
		short find = 0;
		cout << "------ini file begin------" << endl;
		while (!fin.eof())
		{
			//fin>>tmp;
			//cout<<tmp<<endl;
			if (find == 0)
			{
				fin.getline(buff, BUF_SIZE);
				if (strchr(buff, '#') != nullptr)
				{
					continue;
				}
			}
			cout << buff << endl;

			char *start = strchr(buff, '[');
			if (start == nullptr)
			{
				continue;
			}
			char *end = strchr(start, ']');
			char section[ITEM_SIZE] = { '\0' };
			memcpy(section, start + 1, end - start - 1);

			memset(buff, 0, BUF_SIZE);
			map<string, string> itemMap;
			while (!fin.eof())
			{
				fin.getline(buff, BUF_SIZE);
				int len = strlen(buff);
				while (len - 1 >= 0)
				{
					if (buff[len - 1] != ' ')
					{
						break;
					}
					len--;
				}

				if (len - 1 < 0 || strchr(buff, '#') != nullptr)
				{
					continue;
				}

				if (strchr(buff, '[') != nullptr)
				{
					find = 1;
					break;
				}

				char *equal = strchr(buff, '=');
				if (equal == nullptr)
				{
					continue;
				}
				cout << buff << endl;
				char key[ITEM_SIZE] = { 0 };
				memcpy(key, buff, equal - buff);

				itemMap[trim(key)] = trim(equal + 1);
			}

			m_iniMap[trim(section)] = itemMap;
		}
		cout << "------ini file end------" << endl;
		return true;
	}

	bool IniFile::readString(string section, string item, string &value)
	{
		cout << "section:" << section << endl;
		cout << "item:" << item << endl;
		map<string, map<string, string> >::iterator iniIte = m_iniMap.find(trim(section));
		if (iniIte == m_iniMap.end())
		{
			cout << "not can find section" << endl;
			return false;
		}

#if 0
		for (iniIte = m_iniMap.begin(); iniIte != m_iniMap.end(); iniIte++)
		{
			cout << iniIte->first << endl;
			map<string, string>::iterator ite = iniIte->second.begin();
			for (; ite != iniIte->second.end(); ite++)
			{
				cout << "------" << ite->first << ":" << ite->second << endl;
			}
		}
#endif
		map<string, string> m = iniIte->second;
#if 0
		for (map<string, string>::iterator it = m.begin(); it != m.end(); it++)
		{
			cout << "%%%" << it->first << ":::" << it->second << endl;
		}
#endif
		map<string, string>::iterator itemsIte = m.find(trim(item));
#if 0
		for (map<string, string>::iterator it = m.begin(); it != m.end(); it++)
		{
			if (trim(it->first) == trim(item))
			{
				value = it->second;
				cout << "*****************item:" << item << endl;
				cout << "*****************value:" << value << endl;
				break;
			}
		}
#endif

		if (itemsIte != m.end())
		{
			value = itemsIte->second;
			cout << "key:" << itemsIte->first << endl;
			cout << "value:" << itemsIte->second << endl;
			return true;
		}
		else
		{
			cout << "can not find the item" << endl;
			return false;
		}
	}


	bool IniFile::readInt(string section, string item, int &value)
	{
		string tmp;
		if (!readString(section, item, tmp))
		{
			cout << "not find the section or item" << endl;
			return false;
		}

		stringstream str;
		str << tmp;
		str >> value;
		return true;
	}

	string IniFile::trim(const string &str)
	{
		unsigned int spos = 0;
		while (spos != str.size())
		{
			if (str[spos] == ' ')
			{
				spos++;
			}
			else
			{
				break;
			}
		}

		unsigned int epos = str.size() - 1;
		while (epos != 0)
		{
			if (str[epos] == ' ')
			{
				epos--;
			}
			else
			{
				break;
			}
		}

		return str.substr(spos, epos - spos + 1);
	}
}

