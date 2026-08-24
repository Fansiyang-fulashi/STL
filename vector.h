#pragma once
#include<algorithm>
#include<iterator>
#include<iostream>
#include"allocator.h"

template<class T, class Alloc = allocator<T>>
class vector
{
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using iterator = T*;
	using const_iterator = const T*;

public:
	vector(const Alloc& alloc=Alloc())
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
		,_alloc(alloc)
	{
		//std::cout << "调用了构造函数" << std::endl;
	}
     
	vector(size_type n, const value_type& val=value_type(),const Alloc& alloc=Alloc())
		:_start(nullptr)
		,_end(nullptr)
		,_end_of_storage(nullptr)
		,_alloc(alloc)
	{
		//std::cout << "调用了数目构造函数" << std::endl;
		_start = _alloc.allocate(n);
		_end = _start + n;
		_end_of_storage = _start+n;
		for (size_type i = 0; i < n; i++)
			_alloc.construct(_start + i, val);
	}

	template<class InputIterator>
	vector(InputIterator first, InputIterator last, const Alloc& alloc = Alloc(), typename std::iterator_traits<InputIterator>::iterator_category* = nullptr)
	{
		//std::cout << "调用了迭代器区间构造函数" << std::endl;

		size_type n = static_cast<size_type>(std::distance(first, last));
		_start = _alloc.allocate(n);
		_end = _start + n;
		_end_of_storage = _end;
		InputIterator it = first;
		value_type* cur = _start;
		while (it != last)
		{
			_alloc.construct(cur, *it);
			cur++;
			it++;
		}
	}

	vector(const vector<T>& other)
		:_alloc(other.get_allocator())
	{
		//std::cout << "调用了拷贝构造函数" << std::endl;

		size_type n = other.size();
		_start = _alloc.allocate(n);
		_end = _start + n;
		_end_of_storage = _end;
		iterator it = other._start;
		value_type* cur = _start;
		while (it != other.end())
		{
			_alloc.construct(cur, *it);
			cur++;
			it++;
		}
	}

	vector(vector<T>&& other)noexcept
		:_start(nullptr)
		,_end(nullptr)
		,_end_of_storage(nullptr)
		,_alloc(std::move(other.get_allocator()))
	{
		//std::cout << "调用了移动构造函数" << std::endl;

		swap(other);
	}

	vector<T>& operator=(const vector<T>& other)
	{
		//std::cout << "调用了赋值重载函数" << std::endl;

		vector<T> tmp = other;
		swap(tmp);
		//_alloc = other._alloc;  不一定要拷贝依照分配器的特性来看
		return *this;
	}

	vector<T>& operator=(vector<T>&& other) noexcept
	{
		//std::cout << "调用了移动赋值函数" << std::endl;
		if (this != &other)
		{
			iterator it = this->begin();
			while (it != end())
			{
				_alloc.destroy(it);
				it++;
			}
			_alloc.deallocate(begin(),capacity());
			swap(other);
			//_alloc = other._alloc;
		}
		return *this;
	}

	~vector()
	{
		//std::cout << "调用了析构函数" << std::endl;

		iterator it = begin();
		while (it != end())
		{
			_alloc.destroy(it);
			it++;
		}
		_alloc.deallocate(begin(),capacity());
	}

	iterator end()noexcept
	{
		return _end;
	}

	iterator begin()noexcept
	{
		return _start;
	}

	const_iterator end()const noexcept
	{
		return _end;
	}

	const_iterator begin()const noexcept
	{
		return _start;
	}

	size_type size()const noexcept
	{
		return static_cast<size_type>(_end - _start);
	}

	size_type capacity()const noexcept
	{
		return static_cast<size_type>(_end_of_storage - _start);
	}

	void swap(vector<T>& other)noexcept
	{
		std::swap(_start, other._start);
		std::swap(_end, other._end);
		std::swap(_end_of_storage, other._end_of_storage);
	}

	Alloc get_allocator()const noexcept
	{
		return _alloc;
	}

	bool empty()const noexcept
	{
		return _start == _end;
	}

	size_type max_size()const noexcept
	{
		return _alloc.max_size();
	}

	void reserve(size_type n)
	{
		if (n <= capacity())
			return;
		size_type low_size = size();
		vector<T> newp(_alloc);
		newp._start = newp._alloc.allocate(n);
		newp._end = newp._start + low_size;
		newp._end_of_storage = newp._start + n;
		iterator low_it = begin();
		iterator new_it = newp.begin();
		while (low_it != end())
		{
			newp._alloc.construct(new_it, std::move(*low_it));
			low_it++;
			new_it++;
		}
		swap(newp);
	}

	void resize(size_type n, const value_type& val = value_type())
	{
		if (n == size())
			return;
		else if (n > size())
		{
			reserve(n);
			iterator it =end();
			_end = _start + n;
			while (it != end())
			{
				_alloc.construct(it, val);
				it++;
			}
		}
		else
		{
			iterator final =end();
			_end = _start + n;
			iterator it = end();
			while (it != final)
			{
				_alloc.destroy(it);
				it++;
			}
		}
	}

	void shrink_to_fit()
	{
		size_type low_size = size();
		vector<T> newp(_alloc);
		newp._start = newp._alloc.allocate(low_size);//不应该用newp的 newp和low的可能不一致
		newp._end = newp._start + low_size;
		newp._end_of_storage = newp._start + low_size;
		iterator low_it = begin();
		iterator new_it = newp.begin();
		while (low_it != end())
		{
			newp._alloc.construct(new_it, std::move(*low_it));
			low_it++;
			new_it++;
		}
		swap(newp);
	}

private:
	T* _start;
	T* _end;
	T* _end_of_storage;
	Alloc _alloc;
};
