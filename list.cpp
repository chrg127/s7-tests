#include "s7.hpp"
#include <string_view>

void repl(s7::s7 &scheme)
{
    while (1) {
        char buffer[512];
        printf("> ");
        fgets(buffer, 512, stdin);
        if ((buffer[0] != '\n') || (strlen(buffer) > 1)) {
            std::string cmd = "(write " + std::string(buffer) + ")";
            scheme.eval(cmd);
        }
        printf("\n");
    }
}

int main()
{
    s7::s7 scheme;
    auto test = [](s7_scheme *scheme, s7_pointer args) -> s7_pointer {
        s7::s7 &s = *reinterpret_cast<s7::s7 *>(&scheme);
        auto l = s7::List(args);
        printf("size = %d\n", l.size());
        printf("[0] = %d\n", s.to<int>(l.car()));
        printf("[1] = %d\n", s.to<int>(l[1]));
        printf("[2] = %d\n", s.to<int>(l[2]));
        for (const auto &p : l) {
            printf("%s\n", s.to_string(p).data());
        }
        return s7_undefined(scheme);
    };
    scheme.define_function("test", test, 0, 0, true, "test list stuff");
    scheme.eval("(test 1 2 3 4)");
    scheme.repl([](std::string_view s) {
        return s == "q\n";
    });
}

