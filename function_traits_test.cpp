#include "function_traits.hpp"
#include <cstdio>
#include <cassert>

float free2(int x, double b)
{
    return float(x/2 + b);
}

struct SomeStruct {
    int a;

    void method(int x, float y) { printf("%d\n", int(x + y)); }
};

int main()
{
    SomeStruct s;

    auto some_lambda = [&](int x) { return s.a + x; };

    using Traits = FunctionTraits<decltype(free2)>;
    static_assert(std::is_same_v<Traits::ReturnType, float>, "return type should be float");
    static_assert(std::is_same_v<Traits::Argument<0>::Type, int>, "first arg should be int");
    static_assert(std::is_same_v<Traits::Argument<1>::Type, double>, "second arg should be double");

    using TraitsM = FunctionTraits<decltype(SomeStruct::method)>;
    static_assert(std::is_same_v<TraitsM::ReturnType, void>, "return type should be void");

    using TraitsL = FunctionTraits<decltype(some_lambda)>;
    assert(TraitsL::arity == 1 && "arity should be 1");

    return 0;
}

