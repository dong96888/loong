/*
*   File name: CfgFile.h
*	 Describe: 读取配置文件，配置格式如下：
*              aaa aaa值
*			   bbb bbb值
*  Created on: 2015-2-26
*      Author: dengxiudong
*/

#ifndef _CFG_FILE_H_
#define _CFG_FILE_H_



#include <vector>
#include <string>
#include <fstream>

using namespace std;
namespace loong
{
	class CfgFile
	{
	public:
		CfgFile(const char *path);
		bool getIntValue(string item, int & value);
		bool getStrValue(string item, string & value);

	private:
		ifstream m_file;
		vector<string> m_itemVec;
	};
}

#endif
