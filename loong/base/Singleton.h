/*
*   File name: Singletion.h
*    Describe: 获取单体对象
*  Created on: 2015-2-29
*      Author: dengxiudong
*/
namespace loong
{
	template <class T>
	class Singletion
	{
	public:
		//***************************************************************
		// 函数名称:
		// 函数说明：获取单体对象
		// 作    者：dengxiudong
		// 日    期：2015-2-29
		// 返 回 值: 无
		// 参    数: 
		//***************************************************************
		static inline T* instance()
		{
			if (m_instance.get() == nullptr)
			{
				m_instance = make_shard<T>(new T);
			}
			return m_instance.get();
		}
	private:
		Singletion(void) {}
		~Singletion(void) {}

		Singletion(const Singletion&) {}
		Singletion & operator= (const Singletion &) {}

		static shard_ptr<T> m_instance;
	};

	template <class T>
	shard_ptr<T> Singleton<T>::m_instance;
}

/*
/////////////////Singleton应用实例
class MyLog
{
    friend class shard_ptr<MyLog>;
    friend class Singleton<MyLog>; //作为友元可以访问CMyLog的私有成员 
public:
    void Log(char* pszStr)
    {
        cout << "Log msg: " << pszStr << endl;
    }
 
private:
    MyLog(){}; //不允许直接实例化
}

int main()
{
    MyLog *pLog = Singleton<MyLog>::instance();
    pLog->Log("hello word");
 
    system("pause");
    return 0;
}
 */