#include <cstdio>
#include <cstdlib>
#include "s7.hpp"

void test_scheme_defined_function()
{
    s7::s7 scheme;
    scheme["an-integer"] = 1;
    scheme.eval("(define (add1 a) (+ a 1))");
    auto x = scheme["an-integer"].as_opt<int>();
    printf("%d\n", x.value());
    printf("%s\n", std::format("an-integer: {}", scheme["an-integer"].as<int>()).c_str());
    scheme["an-integer"] = 32;
    printf("%s\n", std::format("an-integer: {}", scheme["an-integer"].as_opt<int>().value()).c_str());
    int res = scheme.to<int>(scheme.call("add1", 2));
    printf("%s\n", std::format("(add1 2): {}", res).c_str());

    scheme.define_function("test-sym", [](s7_scheme *sc, s7_pointer _args) -> s7_pointer {
        auto &scheme = *reinterpret_cast<s7::s7 *>(&sc);
        auto args = s7::List(_args);
        auto sym = s7_symbol_table_find_name(scheme.sc, scheme.to<const char *>(args[0]));
        printf("%p\n", sym);
        printf("%s\n", scheme.to_string(sym).c_str());
        return scheme.undefined();
    }, 1, 0, false, "test symbols");

    scheme.repl();
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
    scheme.define("my-pi", 3.14159265);
    scheme.repl();
}

void test_conversion()
{
    s7::s7 scheme;
    auto sym = scheme.define("a-list", scheme.list(1, 2, 3));
    printf("defined %s\n", scheme.to_string(sym).c_str());
    scheme.repl();
}

double add_double(double a, double b) { return a + b; }
int add_int(int x, int y) { return x + y + 1; }

std::string print_append(const std::string &s) {
    printf("a string! %s\n", s.c_str());
    return s + " blah";
}

void test_define_function()
{
    s7::s7 scheme;
    scheme.define_fun_from_ptr("add-double", "doc", add_double);
    scheme.define_fun_from_ptr("add-int", "doc", add_int);
    // scheme.define_fun_from_ptr("str-test", "doc", print_append);
    scheme.repl();
}

int main()
{
    // test_scheme_defined_function();
    // test_c_defined_function();
    // test_conversion();
    test_define_function();
}

