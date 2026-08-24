#include"vector.h"
#include<iostream>

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
	vector<int> a;
	vector<int> b(10, 1);
	vector<int> c(b.begin(), b.end());
	vector<int> d = c;
	vector<int> e = std::move(c);
	a = b;
	c = std::move(b);

	//<3> list

	//<4> AVL平衡树

	//<5> 红黑树

	//<6> 哈希表

	//<7> 算法

	//<8> 智能指针

	//<9> ...

	return 0;
}