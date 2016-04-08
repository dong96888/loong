/*
*   File name: CfgFile.h
*	 Describe: ��ȡ�����ļ������ø�ʽ���£�
*              aaa aaaֵ
*			   bbb bbbֵ
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
