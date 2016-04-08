#include "CfgFile.h"


#include <sstream>
#include <iostream>

#include <stdlib.h>



using namespace std;

namespace loong
{
	CfgFile::CfgFile(const char *path) :m_file(path)
	{
		if (!m_file.is_open())
		{
			cout << "open config file " << path << " failed!" << endl;
			exit(-1);
		}

		while (!m_file.eof())
		{
			string buf;
			m_file >> buf;
			m_itemVec.push_back(buf);
		}
	}

	bool CfgFile::getIntValue(string item, int &value)
	{
		string buf;
		vector<string>::iterator ite = m_itemVec.begin();

		while (ite != m_itemVec.end())
		{
			buf = *ite++;
			if (buf == item)
			{
				value = atoi((*ite).c_str());
				return true;
			}
		}

		return false;
	}

	bool CfgFile::getStrValue(string item, string &value)
	{
		string buf;
		vector<string>::iterator ite = m_itemVec.begin();

		while (ite != m_itemVec.end())
		{
			buf = *ite++;
			if (buf == item)
			{
				value = *ite;
				return true;
			}
		}

		return false;
	}
}

#if 0
int main()
{
	CfgFile cfg("./game.conf");
	int threadnum = 0;
	if (!cfg.getIntValue("thread_num", threadnum))
	{
		cout << "read threadnum failed!" << endl;
	}
	cout << "threadnum: " << threadnum << endl;

	string ip;
	if (!cfg.getStrValue("ip", ip))
	{
		cout << "read ip failed!" << endl;
	}
	cout << "ip: " << ip << endl;

	int port = 0;
	if (!cfg.getIntValue("port", port))
	{
		cout << "read port failed!" << endl;
	}
	cout << "port: " << port << endl;
}
#endif

