/*
*   File name: DBException.h
*	 Describe: MySQL数据库的异常处理类。
*  Created on: 2015-8-5
*      Author: dengxiudong
*/

#ifndef _MYSQLDB_EXCEPTION_H_
#define _MYSQLDB_EXCEPTION_H_


#include <string>
#include <exception>

using namespace std;
namespace loong
{
	class DBException : public std::exception
	{
	public:
		DBException(int errCode, string errMsg = "")//异常派生类构造
		{
			m_errCode = errCode;    //错误代码初始化
			m_errMsg = errMsg;      //错误代码对应消息初始化                                                                                         //对应异常信息初始化
		}
		//拼接了数据库的错误码和错误消息
		string getMsg() const                                                                                                 //派生类what接口重定义
		{
			char tmp[20];                                                                                                  //定义临时字符数组
			snprintf(tmp, sizeof(tmp) - 1, "%d", m_errCode);
			return string(tmp) + ":" + m_errMsg;     //返回连接的错误信息
		}
		int getErrCode() const                       //获取错误代码
		{
			return m_errCode;
		}
		string getErrMsg() const                     //获取错误代码对应信息
		{
			return m_errMsg;
		}
	protected:
		int  m_errCode;                                                                                                 //错误代码
		string  m_errMsg;                            //错误信息
	};
}
#endif