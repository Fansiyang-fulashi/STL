#include"vector.h"
#include"list.h"
#include<iostream>
#include<string>
int main()
{
	//<1> 分配器
	/*allocator<int> give;
	int* a = give.allocate(10);
	for (int i = 0; i < 10; i++)
	{
		give.construct(a+i,i);
	}
	for (int i = 0; i < 10; i++)
	{
		std::cout << a[i] << std::endl;
	}
	give.destory(a);
	give.deallocate(a);*/

	//<2> vector
	/*vector<int> a;
	vector<int> b(10, 1);
	vector<int> c(b.begin(), b.end());
	vector<int> d = c;
	vector<int> e = std::move(c);
	a = b;
	c = std::move(b);*/


	/*vector<int> a;
	a.push_back(1);
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;
	a.erase(a.begin());
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;
	a.shrink_to_fit();
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;
	for (int i = 0; i < 10; i++)
	{
		a.push_back(i);
	}
	a.insert(a.begin()+4, 22);
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;
	for (auto& i : a)
	{
		a.erase(a.begin());
	}
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;
	a.emplace_back(1);
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;
	a.pop_back();
	std::cout << a.size() << std::endl;
	std::cout << a.capacity() << std::endl;

	std::cout << "----------------------" << std::endl;
	vector<std::string> b;
	b.push_back("i am shuaige");
	b.push_back("god");
	b.emplace_back("ijijijij");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	b.push_back("oppop");
	for (auto& i : b)
	{
		std::cout << i << " ";
	}

	std::cout << "---------------" << std::endl;
	vector<vector<std::string>> c;
	c.push_back(b);*/


	//<3> list
	list<int> a;
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);
	a.push_back(1);
	a.push_back(1);
	a.push_back(1);

	a.emplace_back(12);

	list<int> b = a;
	list<int> c = std::move(a);

	auto it = c.begin();
	while (it != c.end())
	{
		std::cout << *it << " ";
		it++;
	}

	//<4> AVL平衡树

	//<5> 红黑树

	//<6> 哈希表

	//<7> 算法

	//<8> 智能指针

	//<9> ...

	return 0;
}