#include<list>
#include<typeinfo>

class IDelegate {
public:
	virtual ~IDelegate() {}
	virtual bool isType(const type_info& _type) = 0;
	virtual void invoke() = 0;
	virtual bool compare(IDelegate *_delegate) const = 0;
};

class StaticDelegate : public IDelegate {
public:
	typedef void(*Func)();
	StaticDelegate(Func _func) : mFunc(_func) { }
	virtual bool isType(const type_info& _type)
	{
		return typeid(StaticDelegate) == _type;
	}
	virtual void invoke()
	{
		mFunc();
	}
	virtual bool compare(IDelegate *_delegate) const
	{
		if (0 == _delegate || !_delegate->isType(typeid(StaticDelegate)))
			return false;
		StaticDelegate *cast = static_cast<StaticDelegate*>(_delegate);
		return cast->mFunc == mFunc;
	}
private:
	Func mFunc;
};

template<class T>
class MethodDelegate : public IDelegate {
public:
	typedef void (T::*Method)();
	MethodDelegate(T *_object, Method _method) : mObject(_object), mMethod(_method) { }
	virtual bool isType(const type_info& _type)
	{
		return typeid(MethodDelegate) == _type;
	}
	virtual void invoke()
	{
		(mObject->*mMethod)();
	}
	virtual bool compare(IDelegate *_delegate) const
	{
		if (0 == _delegate || !_delegate->isType(typeid(MethodDelegate)))
			return false;
		MethodDelegate *cast = static_cast<MethodDelegate*>(_delegate);
		return cast->mObject == mObject && cast->mMethod == mMethod;
	}
private:
	T *mObject;
	Method mMethod;
};

inline IDelegate* new_Delegate(void(*_func)())
{
	return new StaticDelegate(_func);
}

template<class T>
inline IDelegate* new_Delegate(T *_object, void (T::*_method)())
{
	return new MethodDelegate<T>(_object, _method);
}

class MultiDelegate
{
public:
	typedef std::list<IDelegate*> ListDelegate;
	typedef ListDelegate::iterator ListDelegateIterator;
	typedef ListDelegate::const_iterator ConstListDelegateIterator;
	MultiDelegate() { }
	~MultiDelegate()
	{
		clear();
	}
	bool empty() const
	{
		for (ConstListDelegateIterator it = mListDelegates.begin(); it != mListDelegates.end(); ++it)
		{
			if (*it)
				return false;
		}
		return true;
	}
	void clear()
	{
		for (ListDelegateIterator it = mListDelegates.begin(); it != mListDelegates.end(); ++it)
		{
			if (*it)
			{
				delete *it;
				*it = 0;
			}
		}
	}
	MultiDelegate& operator+=(IDelegate* _delegate)
	{
		for (ListDelegateIterator it = mListDelegates.begin(); it != mListDelegates.end(); ++it)
		{
			if ((*it) && (*it)->compare(_delegate))
			{
				delete _delegate;
				return *this;
			}
		}
		mListDelegates.push_back(_delegate);
		return *this;
	}
	MultiDelegate& operator-=(IDelegate* _delegate)
	{
		for (ListDelegateIterator it = mListDelegates.begin(); it != mListDelegates.end(); ++it)
		{
			if ((*it) && (*it)->compare(_delegate))
			{
				if ((*it) != _delegate)
					delete *it;
				(*it) = 0;
				break;
			}
		}
		delete _delegate;
		return *this;
	}
	void operator()()
	{
		ListDelegateIterator it = mListDelegates.begin();
		while (it != mListDelegates.end())
		{
			if (0 == (*it))
			{
				it = mListDelegates.erase(it);
			}
			else
			{
				(*it)->invoke();
				++it;
			}
		}
	}
private:
	ListDelegate mListDelegates;

	MultiDelegate(const MultiDelegate& rhs);
	MultiDelegate& operator=(const MultiDelegate& rhs);
};