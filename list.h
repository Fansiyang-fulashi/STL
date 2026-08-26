#pragma once
#include<algorithm>
#include<iterator>
#include<iostream>
#include"allocator.h"

namespace __hide
{
	template<class T, class Alloc = allocator<T>>
	struct list_node
	{
		template<class U,class _Alloc>
		friend class ::list<U, _Alloc>;

		list_node(const T& val, const Alloc& alloc = Alloc())
			:_prev(nullptr)
			, _next(nullptr)
			, _alloc(alloc)
		{
			_alloc.construct(&_data, val);
		}

		list_node(T&& val, const Alloc& alloc = Alloc())
			:_prev(nullptr)
			, _next(nullptr)
			, _alloc(alloc)
		{
			_alloc.construct(&_data, std::move(val));
		}

		list_node(const list_node& l, const Alloc& alloc = Alloc()) = delete;

		list_node(list_node&& l, const Alloc& alloc = Alloc()) = delete;

		list_node& operator=(const list_node& l) = delete;

		list_node& operator=(list_node&& l) = delete;

		T _data;
		list_node* _prev;
		list_node* _next;
		Alloc _alloc;
	};

	template<class T, class Ref, class Ptr, class Alloc = allocator<T>>
	class list_iterator
	{
		template<class U, class _Alloc>
		friend class ::list<U, _Alloc>;

		using Node = list_node<T, Alloc>;
	public:

	private:
		Node* _cur;
	};
}



template<class T, class Alloc = allocator<T>>
class list
{
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

	using Node = __hide::list_node<T, Alloc>;
	using iterator = __hide::list_iterator<T, T& ,T*, Alloc>;
	using const_iterator = __hide::list_iterator<T, const T&, const T*, Alloc>;

public:

private:
	Node* _head = nullptr;
};