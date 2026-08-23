#pragma once
#include<vector>
#include<exception>

template<class T>
class vector
{
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
public:
	vector()
		:_start(nullptr)
		, _end(nullptr)
		, _end_of_storage(nullptr)
	{
	}
private:
	T* _start;
	T* _end;
	T* _end_of_storage;
};

template<class T>
class allocator
{
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

public:
	template<class U>
	struct rebind
	{
		using other = allocator<U>;
	};

	allocator() noexcept {};

	allocator(const allocator<value_type>& alloc) noexcept {};

	template<class U>
	allocator(const allocator<U>& alloc) noexcept {};

	pointer allocate(size_type n)
	{
		return static_cast<pointer>(::operator new(n * sizeof(value_type)));
	}

	void deallocate(pointer p) noexcept
	{
		::operator delete(p);
	}

	size_type max_size() const noexcept
	{
		return static_cast<size_type>(-1) / sizeof(value_type);
	}

	template<class U,class... Args>
	void construct(U* p, Args&&... args)
	{
		new(p) U(std::forward<Args>(args)...);
	}

	template<class U>
	void destory(U* p) noexcept
	{
		p->~U();
	}

};

template<class T, class U>
bool operator==(const allocator<T>&, const allocator<U>&) noexcept
{
	return true;
}

template<class T, class U>
bool operator!=(const allocator<T>&, const allocator<U>&) noexcept
{
	return false;
}