#ifndef INIFILE_H
#define INIFILE_H

#include <string>
#include <map>
#include <sstream>
#include <iostream>

using namespace std;


namespace loong
{
	struct Item
	{
		string key;
		string value;
	};

	class IniFile
	{
	public:
		bool init(const char *fileName);
		bool readString(string section, string item, string &vaule);
		bool readInt(string section, string item, int &vaule);
		template<class T>
		bool readValue(string section, string item, T &value)
		{
			cout << "begin readValue" << endl;
			string tmp;
			if (!readString(section, item, tmp))
			{
				return false;
			}
			stringstream str;
			str << tmp;
			str >> value;
			return true;
		}
	private:
		map<string, map<string, string> > m_iniMap;
		string trim(const string &str);
	};
}

#endif // INIFILE_H
