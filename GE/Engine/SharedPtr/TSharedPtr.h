#pragma once


template<typename T>
class TSharedPtr
{
public:
	using value_type = T;

	TSharedPtr() 
		:_ptr(nullptr)
	{

	}
	TSharedPtr(T* ptr) { Set(ptr); }

	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// 이동
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }
	// 상속 관계
	template<typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(rhs._ptr); }

	~TSharedPtr() 
	{ 
		Release();
	}

	T* Get()	const
	{
		return _ptr;
	}

public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// 이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool		operator==(T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool		operator!=(T* ptr) const { return _ptr != ptr; }
	bool		operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	T* operator*() { return _ptr; }
	const T* operator*() const { return _ptr; }
	operator T* () const { return _ptr; }
	T* operator->() { return _ptr; }

	T* operator -> () const { return _ptr; }
	//const T* operator->() const { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->Release();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};


//template<typename T>
