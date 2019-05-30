#include <assert.h>
#include <string>
#include <tuple>
#include <array>
#include <utility>
#include <iostream>

namespace simple_delegate
{
class IArguments { 
public: 
	virtual ~IArguments() {}
};

template< typename ...Ts >
class Arguments : public IArguments {
public:
	Arguments(Ts... ts)
	{
		tail = std::make_tuple(std::forward<Ts>(ts)...);
		tuple_size = std::tuple_size<decltype(tail)>::value;
	}
	std::tuple<Ts...> tail;
	size_t tuple_size{ 0 };
};

class IContainer { public: virtual void Call(IArguments*) = 0; };

template< class T, class F, typename... Args>
class Container : public IContainer
{
public:
	typedef void (T::* M)(Args...);

	Container(T* c, F m) : 
		m_class(c),
		m_method(m){}

	void Call(IArguments* i_args)
	{	
		// Џроблема в приведении базового класса к классу наследнику
		// Ќеправильное число параметров
		Arguments<Args...>* a = static_cast<Arguments<Args...>*>(i_args);
		unpack_call(a->tail);
	}

	template<class... Args >
	void unpack_call(const std::tuple<Args...>& t)
	{
		unpack_call_(t, std::index_sequence_for<Args...>{});
	}

	template<class Tuple, std::size_t... Is >
	void unpack_call_(const Tuple& t,
		std::index_sequence<Is...>)
	{
		(m_class->*m_method)(std::get<Is>(t) ...);
	}

private:
	T* m_class; F m_method;
};

class Delegate
{
public:

	Delegate() : m_container(0) {}
	~Delegate() { if (m_container) delete m_container; }

	template< class T, class U >
	void Connect(T* i_class, U i_method)
	{
		if (m_container) delete m_container;
		m_container = new Container< T, U >
			(i_class, i_method);
	}

	template < class... Args>
	void operator()(Args const& ... args)
	{
		Arguments<Args...> t(args...);
		m_container->Call(&t);
	}

private:
	IContainer* m_container;
};

class Victim {
public:
	void Foo() {
		std::cout << "FOO" << std::endl;
	}
	void Bar(int a = 5) {
		std::cout << "BAR - " << "param int = " << a << std::endl;
	}
	void Zoo(int a = 5, char b = 'z') {
		std::cout << "ZOO - " << " param int = " << a
			<< " param char = " << b << std::endl;
	}
	int BarI(int a)
	{
		std::cout << "BAR - " << "param int = " << a << std::endl;
		return 4;
	}
};
}