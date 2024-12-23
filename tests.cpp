#include <cstdio>
#include <cstdlib>
#include "s7.hpp"

void test_scheme_defined_function()
{
    s7::s7 scheme;
    scheme.defvar("an-integer", 1);
    scheme.eval("(define (add1 a) (+ a 1))");
    printf("%s\n", std::format("an-integer: {}", scheme.get_value<int>("an-integer")).c_str());
    scheme.set_value("an-integer", 32);
    printf("%s\n", std::format("an-integer: {}", scheme.get_value<int>("an-integer")).c_str());
    int res = scheme.to<int>(scheme.call("add1", 2));
    printf("%s\n", std::format("(add1 2): {}", res).c_str());
}

s7_pointer add1(s7_scheme *sc, s7_pointer _args)
{
    auto &scheme = *reinterpret_cast<s7::s7 *>(&sc);
    auto args = s7::List(_args);
    printf("add1 called\n");
    if (scheme.is<int>(args[0])) {
        return scheme.from<int>(scheme.to<int>(args[0]) + 1);
    }
    return scheme.wrong_argument_type_error("add1", 1, args[0], "an integer");
}

void test_c_defined_function()
{
    s7::s7 scheme;
    scheme.define_function("add1", add1, 1, 0, false, "(add1 int): adds 1 to int");
    scheme.defvar("my-pi", 3.14159265);
    scheme.repl();
}

void test_conversion()
{
    s7::s7 scheme;
    scheme.defvar("a-list", scheme.list(1, 2, 3));
    scheme.repl();
}

/*
double add_double(double a, double b) { return a + b; }

int test_functions()
{
    s7 s7;
    s7.define_fun_from_ptr("hi", "doc", add_double);
    s7.define_fun_from_ptr("repl", "doc", repl);
}
*/

int main()
{
    // test_scheme_defined_function();
    // test_c_defined_function();
    test_conversion();
}

