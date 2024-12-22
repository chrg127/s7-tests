#include "s7.hpp"
#include <unordered_set>

struct Set {
    std::unordered_set<s7_pointer, s7::Hash, s7::Equal> set;

    explicit Set(s7_scheme *sc)
        : set(std::unordered_set<s7_pointer, s7::Hash, s7::Equal>(512, s7::Hash(sc), s7::Equal(sc))) {}

    void gc_mark(s7::s7 &scheme)
    {
        for (s7_pointer value : set) {
            scheme.mark(value);
        }
    }

    std::string to_string(s7::s7 &scheme)
    {
        std::string str = "#<set(";
        for (const s7_pointer &value : set) {
            str += scheme.to_string(value) + ", ";
        }
        str += ")>";
        return str;
    }
};

template <typename T>
bool is_subset(const T &a, const T &b)
{
    return std::all_of(b.begin(), b.end(), [&](const auto &x) {
        return a.find(x) != a.end();
    });
}

bool operator==(const Set &a, const Set &b)
{
    return a.set.size() == b.set.size() && is_subset(a.set, b.set);
}

s7_pointer set_add(s7_scheme *sc, s7_pointer args)
{
    s7::s7 &scheme = *reinterpret_cast<s7::s7 *>(&sc);
    s7_pointer arg = s7_car(args);
    // if (!scheme.is<Set>(arg)) {
    //     return scheme.wrong_argument_type_error("set-add!", 1, arg, "set");
    // }
    // Set *set = scheme.to<Set>(arg);
    auto opt = scheme.to_opt<Set>(arg);
    if (!opt) {
        return scheme.wrong_argument_type_error("set-add!", 1, arg, "set");
    }
    Set *set = opt.value();
    s7_pointer value = s7_cadr(args);
    set->set.insert(value);
    return scheme.undefined();
}

int main()
{
    s7::s7 scheme;

    scheme.make_c_type<Set>("set");
    scheme.define_function("set-add!", set_add, 2, 0, false, "(set-add! set value) adds value to set");

    scheme.repl();
    return 0;
}

