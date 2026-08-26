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
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	static constexpr size_type FIRST_SIZE = 10;
public:
	vector(const Alloc& alloc = Alloc())
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
		, _alloc(alloc)
	{
		//std::cout << "调用了构造函数" << std::endl;
	}

	vector(size_type n, const value_type& val = value_type(), const Alloc& alloc = Alloc())
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
		, _alloc(alloc)
	{
		//std::cout << "调用了数目构造函数" << std::endl;
		if (n >= max_size())
		{
			throw std::length_error("length too big");
		}
		_start = _alloc.allocate(n);
		_end = _start + n;
		_end_of_storage = _start + n;
		size_type now = 0;
		try
		{
			for (now = 0; now < n; now++)
			{
				_alloc.construct(_start + now, val);//这里只能走拷贝构造 因为这是对于一个对象 也就是只有val本身 当我们使用移动构造之后 这个资源已经被移动走 从而导致后续无资源调用了
			}
		}
		catch (...)
		{
			for (size_type i = 0; i < now; i++)
			{
				_alloc.destroy(_start + i);
			}
			_alloc.deallocate(_start, n);
			throw;
		}
	}

	template<class InputIterator>
	vector(InputIterator first, InputIterator last, const Alloc& alloc = Alloc(), typename std::iterator_traits<InputIterator>::iterator_category* = nullptr)
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
		, _alloc(alloc)
	{
		//std::cout << "调用了迭代器区间构造函数" << std::endl;

		size_type n = static_cast<size_type>(std::distance(first, last));
		_start = _alloc.allocate(n);
		_end = _start + n;
		_end_of_storage = _end;
		InputIterator it = first;
		value_type* constructed = _start;
		try
		{
			while (it != last)
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)//用这个的目的是 强异常保证 就是防止我们在移动的中途突发异常 导致中断 此时怎么也救不会被移动走的资源 这个函数检查的是我们移动的这个对象他的移动构造是否被声明noexcept
				{
					_alloc.construct(constructed, std::move(*it));
				}
				else
				{
					_alloc.construct(constructed, *it);
				}
				constructed++;
				it++;
			}
		}
		catch (...)
		{
			iterator del_it = _start;
			while (del_it != constructed)
			{
				_alloc.destroy(del_it);
			}
			_alloc.deallocate(_start, n);
			throw;
		}
	}

	vector(const vector<T>& other)
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
		, _alloc(other.get_allocator())
	{
		//std::cout << "调用了拷贝构造函数" << std::endl;

		size_type n = other.size();
		_start = _alloc.allocate(n);
		_end = _start + n;
		_end_of_storage = _end;
		iterator it = other._start;
		value_type* cur = _start;
		try
		{
			while (it != other.end())
			{
				_alloc.construct(cur, *it);
				cur++;
				it++;
			}
		}
		catch (...)
		{
			iterator del_it = _start;
			while (del_it != cur)
			{
				_alloc.destroy(del_it);
			}
			_alloc.deallocate(_start, n);
			throw;
		}
	}

	vector(vector<T>&& other) noexcept
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
		, _alloc(std::move(other.get_allocator()))
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
		if (this == &other) return *this;
		swap(other);
		return *this;
	}

	~vector() noexcept
	{
		//std::cout << "调用了析构函数" << std::endl;

		iterator it = begin();
		while (it != end())
		{
			_alloc.destroy(it);
			it++;
		}
		_alloc.deallocate(begin(), capacity());
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

	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rend() const noexcept
	{
		return reverse_iterator(begin());
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
		if (n >= max_size())
		{
			throw std::length_error("length too big");
		}
		size_type low_size = size();
		pointer new_ptr = _alloc.allocate(n);//这里不需要捕获 有异常会自动跳过下面的代码 直接到下面的catch 
		pointer constructed = new_ptr;
		try
		{
			iterator it = begin();
			while (it != end())
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)//if constexpr C++17 用于在编译期间判断 不满足的代码会不生成
				{
					_alloc.construct(constructed, std::move(*it));
				}
				else
				{
					_alloc.construct(constructed, *it);
				}
				constructed++;
				it++;
			}
		}
		catch (...)
		{
			pointer ptr = new_ptr;
			while (ptr != constructed)
			{
				_alloc.destroy(ptr);
				ptr++;
			}
			_alloc.deallocate(new_ptr, n);
			throw;
		}
		iterator it = begin();
		while (it != end())
		{
			_alloc.destroy(it);
			it++;
		}
		_alloc.deallocate(_start, capacity());
		_start = new_ptr;
		_end = _start + low_size;
		_end_of_storage = _start + n;
	}

	void resize(size_type n, const value_type& val = value_type())
	{
		if (n > max_size())
		{
			throw std::length_error("length too big");
		}
		if (n == size())
			return;
		else if (n > size())
		{
			reserve(n);
			iterator it = end();
			try
			{
				while (it != _start + n)
				{
					_alloc.construct(it, val);
					it++;
				}
			}
			catch (...)
			{
				iterator del_it = end();
				while (del_it != it)
				{
					_alloc.destroy(del_it); 
					del_it++;
				}
				throw;
			}
				_end = _start + n;
		}
		else
		{
			iterator final = end();
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
		pointer new_ptr = _alloc.allocate(low_size);
		pointer constructed = new_ptr;
		try
		{
			iterator it = begin();
			while (it != end())
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)
				{
					_alloc.construct(constructed, std::move(*it));
				}
				else
				{
					_alloc.construct(constructed, *it);
				}
				constructed++;
				it++;
			}
		}
		catch (...)
		{
			pointer ptr = new_ptr;
			while (ptr != constructed)
			{
				_alloc.destroy(ptr);
				ptr++;
			}
			_alloc.deallocate(new_ptr, low_size);
			throw;
		}
		iterator it = begin();
		while (it != end())
		{
			_alloc.destroy(it);
		}
		_alloc.deallocate(_start, capacity());
		_start = new_ptr;
		_end = _start + low_size;
		_end_of_storage = _start + low_size;
	}

	reference operator[](size_type n) noexcept
	{
		return *(_start + n);
	}

	const_reference operator[](size_type n) const noexcept
	{
		return *(_start + n);
	}

	reference at(size_type n)
	{
		if (n >= size())
			throw std::out_of_range("vector at out of range");
		return *(_start + n);
	}

	const_reference at(size_type n) const
	{
		if (n >= size())
			throw std::out_of_range("vector at out of range");
		return *(_start + n);
	}

	reference front() noexcept
	{
		return *_start;
	}

	const_reference front() const noexcept
	{
		return *_start;
	}

	reference back() noexcept
	{
		return *(_end - 1);
	}

	const_reference back() const noexcept
	{
		return *(_end - 1);
	}

	void push_back(const T& val)
	{
		if (size() + 1 > capacity())
		{
			size() == 0 ? reserve(FIRST_SIZE) : reserve(size() * 2);
		}
		_alloc.construct(_end, val);	
		_end++;
	}

	void push_back(T&& val)
	{
		if (size() + 1 > capacity())
		{
			size() == 0 ? reserve(FIRST_SIZE) : reserve(size() * 2);
		}
		_alloc.construct(_end, std::move(val));
		_end++;
	}

	void pop_back() noexcept
	{
		_alloc.destroy(_end - 1);
		_end--;
	}

	iterator insert(iterator pos, const T& val)
	{
		if (size() + 1 > capacity())
		{
			size_type offset = pos - begin();
			size() == 0 ? reserve(FIRST_SIZE) : reserve(size() * 2);
			pos = begin() + offset;//扩容后更新
		}
		iterator it = _end;
		try
		{
			while (it != pos)
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)
				{
					_alloc.construct(it, std::move(*(it - 1)));
				}
				else
				{
					_alloc.construct(it, *(it - 1));
				}
				_alloc.destroy(it - 1);
				it--;
			}
		}
		catch (...)
		{
			throw;//无法做到强异常安全
		}
		_alloc.construct(pos, val);
		_end++;
		return pos;
	}

	iterator insert(iterator pos, T&& val)
	{
		if (size() + 1 > capacity())
		{
			size_type offset = pos - begin();
			size() == 0 ? reserve(FIRST_SIZE) : reserve(size() * 2);
			pos = begin() + offset;//扩容后更新
		}
		iterator it = _end;
		try
		{
			while (it != pos)
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)
				{
					_alloc.construct(it, std::move(*(it - 1)));
				}
				else
				{
					_alloc.construct(it, *(it - 1));
				}
				_alloc.destroy(it - 1);
				it--;
			}
		}
		catch (...)
		{
			throw;//无法做到强异常安全
		}
		try
		{
			_alloc.construct(pos, std::move(val));
		}
		catch (...)
		{
			throw;//无法做到强异常安全
		}
		_end++;
		return pos;
	}

	iterator insert(iterator pos, size_type n, const T& val)//这个没有右值版本 因为这个要构造多个对象 在构造第一个的时候直接就是没有资源了
	{
		if (n > max_size())
		{
			throw std::length_error("length too big");
		}
		if (size() + n > capacity())
		{
			size_type offset = pos - begin();
			size() == 0 ? reserve(FIRST_SIZE + n) : reserve(size() * 2 + n);
			pos = begin() + offset;
		}
		iterator it = _end + n - 1;
		try
		{
			while (it != pos + n - 1)
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)
				{
					_alloc.construct(it, std::move(*(it - n)));
				}
				else
				{
					_alloc.construct(it, *(it - n));
				}
				_alloc.destroy(it - n);
				it--;
			}
		}
		catch (...)
		{
			throw;//无法做到强异常安全
		}
		try
		{
			size_type now = 0;
			for (size_type now = 0; now < n; now++)
			{
				_alloc.construct(pos + now, val);
			}
		}
		catch (...)
		{
			throw;
		}
		_end += n;
		return pos;
	}

	iterator erase(iterator pos)
	{
		iterator it = pos;
		try
		{
			while (it != end()-1)
			{
				if constexpr (std::is_nothrow_move_assignable_v<T>)
				{
					*it=std::move(*(it + 1));//全是在已经初始化的内存上实现的
				}
				else
				{
					*it = *(it + 1);
				}
				it++;
			}
		}
		catch (...)
		{
			throw;//无法做到强异常安全
		}
		_alloc.destroy(it);
		_end--;
		return pos;
	}

	iterator erase(iterator first, iterator last)
	{
		iterator it = first;
		size_type n = last - first;
		try
		{
			while (it != end()-n)
			{
				if constexpr (std::is_nothrow_move_assignable_v<T>)//判断移动赋值是否会出异常
				{
					*it = std::move(*(it + n));
				}
				else
				{
					*it = *(it + n);
				}
				it++;
			}
		}
		catch (...)
		{
			throw;//无法做到强异常安全
		}
		while (it != end())
		{
			_alloc.destroy(it);
			it++;
		}
		_end -= n;
		return first;
	}

	void clear() noexcept
	{
		iterator it = begin();
		while (it != end())
		{
			_alloc.destroy(it);
			it++;
		}
		_end = begin();
	}

	template<class... Args>
	reference emplace_back(Args&&... args)
	{
		if (size() + 1 > capacity())
		{
			size() == 0 ? reserve(FIRST_SIZE) : reserve(size() * 2);
		}
		_alloc.construct(_end, std::forward<Args>(args)...);
		_end++;
		return *(_end - 1);
	}

private:
	T* _start;
	T* _end;
	T* _end_of_storage;
	Alloc _alloc;
};