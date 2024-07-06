#pragma once

#include <cstddef>
#include <tuple>

// --------- Call functor on all elements of a tuple ---------------------------

// ---------- Non-const version ---------

template <typename F, typename... Args>
static void call_elem(F f, Args&... args)
{
	auto x = {(f(args), 0)...};
}

template <typename Functor, typename... Args>
static void call_elems(Functor func, Args&... args)
{
	call_elem([&](auto& arg) { func(arg); }, args...);
}

template <typename Functor, typename Tuple, size_t... Is>
static void call_elems_in_tuple_i(Functor func, Tuple& t,
								  std::index_sequence<Is...>)
{
	call_elems(func, std::get<Is>(t)...);
}

template <typename Functor, typename... GivenTypes>
static void call_elems_in_tuple(Functor func, std::tuple<GivenTypes...>& t)
{
	constexpr size_t N = std::tuple_size_v<std::tuple<GivenTypes...>>;
	std::make_index_sequence<N> a;
	call_elems_in_tuple_i(func, t, a);
}

// ------------ const version ----------

template <typename F, typename... Args>
static void call_elem(F f, const Args&... args)
{
	auto x = {(f(args), 0)...};
}

template <typename Functor, typename... Args>
static void call_elems(Functor func, const Args&... args)
{
	call_elem([&](auto& arg) { func(arg); }, args...);
}

template <typename Functor, typename Tuple, size_t... Is>
static void call_elems_in_tuple_i(Functor func, const Tuple& t,
								  std::index_sequence<Is...>)
{
	call_elems(func, std::get<Is>(t)...);
}

template <typename Functor, typename... GivenTypes>
static void call_elems_in_tuple(Functor func, const std::tuple<GivenTypes...>& t)
{
	constexpr size_t N = std::tuple_size_v<std::tuple<GivenTypes...>>;
	std::make_index_sequence<N> a;
	call_elems_in_tuple_i(func, t, a);
}
