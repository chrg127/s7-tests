#pragma once

#include <utility>
#include <tuple>

template <typename F>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)> : public FunctionTraits<R(Args...)>
{};

template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
    using ReturnType = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct Argument {
        static_assert(N < arity, "error: invalid parameter index.");
        using Type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

// member function pointer
template <typename C, typename R, typename... Args>
struct FunctionTraits<R(C::*)(Args...)> : public FunctionTraits<R(C&, Args...)> {};

// const member function pointer
template <typename C, typename R, typename... Args>
struct FunctionTraits<R(C::*)(Args...) const> : public FunctionTraits<R(C&, Args...)> {};

// member object pointer
template <typename C, typename R>
struct FunctionTraits<R(C::*)()> : public FunctionTraits<R(C&)> {};

// functor
template <typename F>
class FunctionTraits {
    using CallType = FunctionTraits<decltype(&F::operator())>;
public:
    using ReturnType = typename CallType::ReturnType;

    static constexpr std::size_t arity = CallType::arity - 1;

    template <std::size_t N>
    struct Argument {
        static_assert(N < arity, "error: invalid parameter index.");
        using Type = typename CallType::template Argument<N+1>::Type;
    };
};

template <typename F>
struct FunctionTraits<F &> : public FunctionTraits<F> {};

template <typename F>
struct FunctionTraits<F &&> : public FunctionTraits<F> {};

