#include <assert.h>
#include <string>
#include <tuple>
#include <array>
#include <utility>
#include <iostream>
#include <functional>
#include <utility>

namespace simple_delegate
{
	template<std::size_t... Indices>
	struct indices
	{
		using type = indices<Indices...>;
	};

	template<std::size_t, typename Indices>
	struct append_index;

	template<std::size_t I, std::size_t... Indices>
	struct append_index< I, indices<Indices...> > : indices< Indices..., I > {};

	template<std::size_t N>
	struct make_indices : append_index< N - 1, typename make_indices<N - 1>::type > {};

	template<>
	struct make_indices<1> : indices<0> {};

	template<std::size_t, typename T>
	struct arg
	{
		explicit arg(T val) : value(val) {}

		T const value;
	};

	template<typename, typename...>
	struct args_list_impl;

	template<std::size_t... Indices, typename... Args>
	struct args_list_impl<indices<Indices...>, Args...> : arg<Indices, Args>...
	{
		template<typename... OtherArgs>
		args_list_impl(OtherArgs&& ... args) : arg<Indices, Args>(std::forward<OtherArgs>(args))... {}
	};

	template<typename... Args>
	struct args_list : args_list_impl< typename make_indices< sizeof...(Args)>::type, Args... >
	{
		using base_t = args_list_impl< typename make_indices< sizeof...(Args) >::type, Args... >;

		template<typename... OtherArgs>
		args_list(OtherArgs&& ... args) : base_t(std::forward<OtherArgs>(args)...) {}
	};

	template<std::size_t I, typename Head, typename... Tail>
	struct type_at_index
	{
		using type = typename type_at_index<I - 1, Tail...>::type;
	};

	template<typename Head, typename... Tail>
	struct type_at_index<0, Head, Tail...>
	{
		using type = Head;
	};

	template<std::size_t I, typename... Args>
	using type_at_index_t = typename type_at_index<I, Args...>::type;

	template<std::size_t I, typename... Args>
	type_at_index_t<I, Args...> get_arg(args_list<Args...> const& args)
	{
		arg< I, type_at_index_t<I, Args...> > const& argument = args;
		return argument.value;
	};

	template<typename T, typename S>
	T take_argument(T const& arg, S const& args)
	{
		return arg;
	}

	template<typename T, typename... Args,
		std::size_t I = std::is_placeholder<T>::value,
		typename = typename std::enable_if< I != 0 >::type >
		type_at_index_t<I - 1, Args...> take_argument(T const& ph, args_list<Args...> const& args)
	{
		return get_arg< I - 1, Args... >(args);
	}
	
	//-------------------------------------------------------------------------------
	template<bool B, typename T, typename F >
	using conditional_t = typename std::conditional<B, T, F>::type;

	template<typename Func, typename... BinderArgs>
	struct binder
	{
		binder(Func const& func, BinderArgs&& ... binderArgs)
			: m_func{ func }
			, m_args{ std::forward<BinderArgs>(binderArgs)... }
		{}

		template<typename... Args>
		void operator()(Args&& ... args) const
		{
			// need check: sizeof...(Args) should not be less than max placeholder value
			call_function(make_indices< sizeof...(BinderArgs) >{}, std::forward<Args>(args)...);
		}

	private:
		template< std::size_t... Indices, typename... Args >
		void call_function(indices<Indices...> const&, Args&& ... args) const
		{
			struct empty_list
			{
				empty_list(Args&& ... args) {}
			};

			using args_t = conditional_t< sizeof...(Args) == 0, empty_list, args_list<Args...> >;
			args_t const argsList{ std::forward<Args>(args)... };

			m_invoker.invoke(m_func, take_argument(get_arg<Indices, BinderArgs...>(m_args), argsList)...);
		}

		struct free_function_invoker
		{
			template<typename...Args>
			void invoke(Func const& func, Args&& ... args) const
			{
				return func(std::forward<Args>(args)...);
			}
		};

		struct member_function_invoker
		{
			template<typename ObjType, typename...Args>
			void invoke(Func const& func, ObjType&& obj, Args&& ... args) const
			{
				return (obj.*func)(std::forward<Args>(args)...);
			}
		};

		using invoker_t =
			conditional_t< std::is_member_function_pointer<Func>::value, member_function_invoker, free_function_invoker >;

		invoker_t m_invoker;
		Func m_func;
		args_list<BinderArgs...> m_args;
	};

	template<typename Func, typename... BinderArgs>
	binder<Func, BinderArgs...> bind(Func const& func, BinderArgs&& ... args)
	{
		return binder<Func, BinderArgs...>(func, std::forward<BinderArgs>(args)...);
	}

	template<typename Func, typename... BinderArgs>
	binder<Func, BinderArgs...> bind(Func const& func, BinderArgs&& ... args)
	{
		return binder<Func, BinderArgs...>(func, std::forward<BinderArgs>(args)...);
	}
}