#include "DelegateTemplate/FunctionDispatcher.h"
#include "DelegateTemplate/Dispatcher.h"
#include "DelegateTemplate/Utils.h"
#include "VerySimpleDelegate/Delegate.h"
#include <iostream>
#include <chrono>
#include <type_traits>
#include <typeinfo>

using namespace std;

int global(int a, float b)
{
	return a + static_cast<int>(b);
}

struct UserStruct
{
	int member(int a, float b)
	{
		return a + static_cast<int>(b);
	}
};

class Victim {
public:
	Victim() {};
	~Victim() {};
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

void test_simple_delegate()
{
	Victim test_class;
	//test_delegate.Connect(&test_class, &simple_delegate::Victim::Foo);
	//test_delegate();
	auto test_delegate = bind(&Victim::BarI, test_class, 10);
	test_delegate();
	//test_delegate.Connect(&test_class, &simple_delegate::Victim::Zoo);
	//test_delegate(10, 'z');
	//test_delegate.Connect(&test_class, &simple_delegate::Victim::BarI);
}


int main()
{	

	test_simple_delegate();
	const int NUM_TESTS{ 10 };
	const int NUM_ITERATIONS{ 100 };

	Dispatcher<int(int, float)> dispatcher;
	FunctionDispatcher<int(int, float)> functionDispatcher;

	auto ptr = make_shared_lambda([](int a, float b)->int
	{
		return a + static_cast<int>(b);
	});

	volatile int result{ 0 };

	cout << "lambda test raw" << "\n";

	dispatcher.add(ptr);
	functionDispatcher.add("lambda", ptr);

	// lambda test raw
	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += (*ptr)(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "lambda test dispatcher" << "\n";

	// lambda test dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			dispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "lambda test function dispatcher" << "\n";

	// lambda test function dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{			functionDispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	dispatcher.remove(ptr);
	functionDispatcher.remove("lambda");

	cout << "global test raw" << "\n";

	dispatcher.add<&global>();
	functionDispatcher.add<&global>("global");

	// global test raw
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += global(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "global test dispatcher" << "\n";

	// global test dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			dispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "global test function dispatcher" << "\n";

	// lambda test function dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			functionDispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	dispatcher.remove<&global>();
	functionDispatcher.remove("global");

	cout << "member test raw" << "\n";

	auto ptr2 = make_shared<UserStruct>();
	dispatcher.add<UserStruct, & UserStruct::member>(ptr2);
	functionDispatcher.add<UserStruct, & UserStruct::member>("UserStruct::member", ptr2);

	// member test raw
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += ptr2->member(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "member test dispatcher" << "\n";

	// member test dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			dispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "member test function dispatcher" << "\n";

	// member test function dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			functionDispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	dispatcher.remove<UserStruct, & UserStruct::member>(ptr2);
	functionDispatcher.remove("UserStruct::member");

	/*dispatcher.add(ptr);
	functionDispatcher.add("lambda", ptr);
	dispatcher.add<&global>();
	functionDispatcher.add<&global>("global");
	dispatcher.add<UserStruct, &UserStruct::member>(ptr2);
	functionDispatcher.add<UserStruct, &UserStruct::member>("UserStruct::member", ptr2);
	dispatcher(5, 10.0f);
	cout << "\n";
	functionDispatcher(5, 10.0f);*/

	cin.get();

	return result;
}


//template< class T, class Res, class... Args>
//class Container<T, Res, Res(T::*)(Args...)> : public IContainer
//{
//public:
//	typedef Res(T::* M)(Args...);
//	
//	Container(T* c, M m, std::tuple<Args...> arguments) :
//		m_class(c),
//		m_method(m),
//		arguments_(arguments)
//	{
//		cout << "ctor";
//	}
//
//	void Call()
//	{
//		unpack_call(arguments_);
//	}
//
//	template<class... Args >
//	void unpack_call(const std::tuple<Args...>& t)
//	{
//		unpack_call_(t, std::index_sequence_for<Args...>{});
//	}
//
//	template<class Tuple, std::size_t... Is >
//	void unpack_call_(const Tuple& t,
//		std::index_sequence<Is...>)
//	{
//		ret_val = (m_class->*m_method)(std::get<Is>(t) ...);
//	}
//
//	T* m_class; 
//	M m_method;
//	std::tuple<Args...> arguments_;
//	Res ret_val;
//};