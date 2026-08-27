#pragma once
#include<algorithm>
#include<iterator>
#include<iostream>
#include"allocator.h"

template<class T, class Alloc> 
class list;

namespace __hide
{
	template<class T>
	struct list_node
	{
		template<class U,class _Alloc>
		friend class ::list;

		list_node()
			:_prev(nullptr)
			, _next(nullptr)
		{
		}

		list_node(const T& val)
			:_prev(nullptr)
			,_next(nullptr)
			,_data(val)
		{
		}

		list_node(T&& val)
			:_prev(nullptr)
			,_next(nullptr)
			,_data(std::move(val))
		{
		}

		list_node(const list_node& l) = delete;

		list_node(list_node&& l) = delete;

		list_node& operator=(const list_node& l) = delete;

		list_node& operator=(list_node&& l) = delete;

		T _data;
		list_node* _prev;
		list_node* _next;
	};

	template<class T, class Ref, class Ptr>
	class list_iterator
	{
		template<class U, class _Alloc>
		friend class ::list;

		using Node = list_node<T>;
		using Self = list_iterator<T, Ref, Ptr>;
	public:
		list_iterator(Node* node)
			:_cur(node)
		{
		}

		Self& operator++() noexcept
		{
			_cur=_cur->_next;
			return *this;
		}

		Self& operator--() noexcept
		{
			_cur= _cur->_prev;
			return *this;
		}

		Self& operator++(int) noexcept
		{
			Self tmp = *this;
			_cur = _cur->_next;
			return tmp;
		}

		Self& operator--(int) noexcept
		{
			Self tmp = *this;
			_cur = _cur->_prev;
			return *tmp;
		}

		Ref operator*() noexcept
		{
			return _cur->_data;
		}

		Ptr operator->() noexcept
		{
			return &(_cur->_data);
		}

		bool operator==(const Self& it) const noexcept
		{
			return _cur == it._cur;
		}

		bool operator!=(const Self& it) const noexcept
		{
			return _cur != it._cur;
		}

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

	using Node = __hide::list_node<T>;
	using iterator = __hide::list_iterator<T, T& ,T*>;
	using const_iterator = __hide::list_iterator<T, const T&, const T*>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using Node_Alloc = typename Alloc::template rebind<Node>::other;

public:
	list(const Node_Alloc& alloc=Node_Alloc())
		:_alloc(alloc)
	{
		init();
	}

	list(const list& l)
		:_alloc(l._alloc)
		,_size(0)
	{
		init();
		Node* new_node = nullptr;
		Node* cur_node = _head;
		const_iterator it = l.begin();
		try
		{
			while (it != l.end())
			{
				new_node = _alloc.allocate(1);
				_alloc.construct(new_node, *it);
				cur_node->_next = new_node;
				new_node->_prev = cur_node;
				cur_node = new_node;
				it++;
			}
		}
		catch (...)
		{
			_head->_prev = cur_node;
			cur_node->_next = _head;
			clear();
			throw;
		}
		_size = l._size;//要在确认没有异常后才赋值
		_head->_prev = cur_node;
		cur_node->_next = _head;
	}

	list(size_type n, const T& val, const Node_Alloc& alloc = Node_Alloc())
		:_alloc(alloc)
		,_size(0)
	{
		init();
		Node* new_node = nullptr;
		Node* cur_node = _head;
		try
		{
			for (size_type i = 0; i < n; i++)
			{
				new_node = _alloc.allocate(1);
				_alloc.construct(new_node, val);
				cur_node->_next = new_node;
				new_node->_prev = cur_node;
				cur_node = new_node;
			}
		}
		catch (...)
		{
			//回滚
		}
		_size = n;
		_head->_prev = cur_node;
		cur_node->_next = _head;
	}

	template<class InputIterator>
	list(InputIterator first, InputIterator last, const Node_Alloc& alloc = Node_Alloc())
		:_alloc(alloc)
		, _size(0)
	{
		init();
		InputIterator it = first;
		Node* new_node = nullptr;
		Node* cur_node = _head;
		try
		{
			while (it != last)
			{
				new_node = _alloc.allocate(1);
				_alloc.construct(new_node, *it);
				cur_node->_next = new_node;
				new_node->_prev = cur_node;
				cur_node = new_node;
				it++;
			}
		}
		catch (...)
		{
			//回滚
		}
		_size = static_cast<size_type>(std::distance(first, last));
		_head->_prev = cur_node;
		cur_node->_next = _head;
	}

	list(list<T,Alloc>&& l)
		:_head(nullptr)
		, _size(0)
	{
		swap(l);
	}

	list<T, Alloc>& operator=(list<T, Alloc> l)
	{
		swap(l);
		return *this;
	}

	list<T, Alloc>& operator=(list<T, Alloc>&& l)
	{
		list<T, Alloc> tmp = std::move(l);
		swap(tmp);
		return *this;
	}

	~list() noexcept
	{
		if (_head == nullptr)
			return;
		clear();
		_alloc.destroy(_head);
		_alloc.deallocate(_head, 1);
	}

	void clear() noexcept
	{
		Node* del_node = _head->_next;
		Node* next_node = nullptr;
		while (del_node!=_head)
		{
			next_node = del_node->_next;
			_alloc.destroy(del_node);
			_alloc.deallocate(del_node, 1);
			del_node = next_node;
		}
		_head->_next = _head;
		_head->_prev = _head;
		_size = 0;
	}

	void init()
	{
		_head = _alloc.allocate(1);
		_alloc.construct(_head);
		_head->_prev = _head;
		_head->_next = _head;
	}

	iterator begin() noexcept
	{
		return _head->_next;
	}

	iterator end() noexcept
	{
		return _head;
	}

	const_iterator begin()const noexcept
	{
		return _head->_next;
	}

	const_iterator end()const noexcept
	{
		return _head;
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rbegin() const
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rend() const 
	{
		return reverse_iterator(begin());
	}

	bool empty() const
	{
		return _size == 0;
	}

	size_type size() const
	{
		return _size;
	}

	reference front()
	{
		return *(_head->_next);
	}

	const_reference front() const 
	{
		return *(_head->_next);
	}

	reference back()
	{
		return *(_head->_prev);
	}

	const_reference back() const 
	{
		return *(_head->_prev);
	}

	void swap(list<T,Alloc>& l)
	{
		std::swap(_head, l._head);
		std::swap(_size, l._size);
	}

	void push_front(const T& val)
	{
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, val);
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}
		Node* next_node = _head->_next;
		_head->_next = new_node;
		new_node->_prev = _head;
		next_node->_prev = new_node;
		new_node->_next = next_node;
		_size++;
	}

	void push_front(T&& val)
	{
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, std::move(val));
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}	
		Node* next_node = _head->_next;
		_head->_next = new_node;
		new_node->_prev = _head;
		next_node->_prev = new_node;
		new_node->_next = next_node;
		_size++;
	}

	void push_back(T&& val)
	{
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, std::move(val));
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}		
		Node* prev_node = _head->_prev;
		prev_node->_next = new_node;
		new_node->_prev = prev_node;
		new_node->_next = _head;
		_head->_prev = new_node;
		_size++;
	}

	void push_back(const T& val)
	{
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, val);
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}
		Node* prev_node = _head->_prev;
		prev_node->_next = new_node;
		new_node->_prev = prev_node;
		new_node->_next = _head;
		_head->_prev = new_node;
		_size++;
	}

	template<class... Args>
	void emplace_back(Args&&... args)
	{
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, std::forward<Args>(args)...);
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}
		Node* prev_node = _head->_prev;
		prev_node->_next = new_node;
		new_node->_prev = prev_node;
		new_node->_next = _head;
		_head->_prev = new_node;
		_size++;
	}

	template<class... Args>
	void emplace_front(Args&&... args)
	{
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, std::forward<Args>(args)...);
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}
		Node next_node = _head->_next;
		_head->_next = new_node;
		new_node->_prev = _head;
		next_node->_prev = new_node;
		new_node->_next = next_node;
		_size++;
	}

	iterator insert(iterator pos, const T& val)
	{
		pos--;
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, val);
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}
		Node* pos_node = pos._cur;
		Node* next_node = pos_node->_next;
		pos_node->_next = new_node;
		new_node->_prev = pos_node;
		new_node->_next = next_node;
		next_node->_prev = new_node;
		_size++;
		return pos_node;
	}

	iterator insert(iterator pos, T&& val)
	{
		pos--;
		Node* new_node = _alloc.allocate(1);
		try
		{
			_alloc.construct(new_node, std::move(val));
		}
		catch (...)
		{
			_alloc.destroy(new_node);
			_alloc.deallocate(new_node, 1);
			throw;
		}
		Node* pos_node = pos._cur;
		Node* next_node = pos_node->_next;
		pos_node->_next = new_node;
		new_node->_prev = pos_node;
		new_node->_next = next_node;
		next_node->_prev = new_node;
		_size++;
		return pos_node;
	}

	iterator erase(iterator pos)
	{
		Node* del_node = pos._cur;
		Node* prev_node = del_node->_prev;
		Node* next_node = del_node->_next;
		_alloc.destroy(del_node);
		_alloc.deallocate(del_node, 1);
		prev_node->_next = next_node;
		next_node->_prev = prev_node;
		_size--;
		return next_node;
	}

	void pop_front()
	{
		erase(begin());
	}

	void pop_back()
	{
		erase(--end());
	}

	//....

private:
	Node* _head=nullptr;
	size_type _size=0;
	Node_Alloc _alloc;
};