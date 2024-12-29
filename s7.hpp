#include <cstring>
#include <cassert>
#include <string>
#include <string_view>
#include <format>
#include <type_traits>
#include <memory>
#include <functional>
#include <unordered_map>
#include <optional>
#include "s7/s7.h"

namespace s7 {

struct Equal {
    s7_scheme *sc;

    bool operator()(const s7_pointer &a, const s7_pointer &b) const {
        return s7_is_equal(sc, a, b);
    }
};

struct Hash {
    s7_scheme *sc;

    size_t operator()(const s7_pointer& p) const
    {
        return s7_hash_code(sc, p, s7_name_to_value(sc, "equal?"));
    }
};

template <typename T>
struct TypeTag {
    static inline int tag = -1;
};

class List {
    s7_pointer p;

public:
    explicit List(s7_pointer p) : p{p} {}

    s7_pointer operator[](size_t i) const
    {
        s7_pointer x = this->p;
        while (i-- > 0) {
            x = s7_cdr(x);
        }
        return s7_car(x);
    }

    s7_pointer car() const { return s7_car(p); }
    List cdr() const { return List(s7_cdr(p)); }
    const s7_pointer ptr() const { return p; }
    s7_pointer ptr() { return p; }

    size_t size() const
    {
        size_t s = 0;
        for (auto p = this->p; s7_is_pair(p); p = s7_cdr(p), s++)
            ;
        return s;
    }

    struct iterator {
        s7_pointer p = nullptr;

        using value_type = s7_pointer;

        iterator() = default;
        explicit iterator(s7_pointer p) : p{p} {}
        iterator & operator++() { p = s7_cdr(p); return *this; }
        iterator operator++(int) { auto i = *this; p = s7_cdr(p); return i; }
        s7_pointer operator*() const { return s7_car(p); }
    };

    iterator begin() { return iterator(p); }
    iterator end() { return iterator(); }
};

bool operator==(const List::iterator &a, const List::iterator &b)
{
    return (a.p == nullptr || !s7_is_pair(a.p)) && (b.p == nullptr || !s7_is_pair(b.p))
        || (a.p != nullptr && b.p != nullptr && s7_is_eq(a.p, b.p));
}

struct Variable;

struct s7 {
    s7_scheme *sc;

    s7() : sc(s7_init()) {}
    ~s7() { s7_free(sc); }

    s7(const s7 &) = delete;
    s7 & operator=(const s7 &) = delete;
    s7(s7 &&other) { operator=(std::move(other)); }
    s7 & operator=(s7 &&other) { this->sc = other.sc; other.sc = nullptr; return *this; }

    /* eval/repl stuff */
    s7_pointer eval(std::string_view code)
    {
        return s7_eval_c_string(sc, code.data());
    }

    void repl(
        std::function<bool(std::string_view)> quit = [](std::string_view) { return false; },
        std::function<void(std::string_view)> output = [](std::string_view s) {
            printf("%s", s.data());
        },
        std::function<std::string()> input = []() {
            char buffer[512];
            fgets(buffer, sizeof(buffer), stdin);
            return std::string(buffer);
        }
    )
    {
        for (;;) {
            output("> ");
            auto s = input();
            if (quit(s)) {
                break;
            }
            if (s[0] != '\n' || s.size() > 1) {
                output(to_string(eval(s)));
            }
            output("\n");
        }
    }

    /* gc stuff */
    void mark(s7_pointer p)
    {
        s7_mark(p);
    }

    /* constants */
    s7_pointer undefined() { return s7_undefined(sc); }
    s7_pointer nil() { return s7_nil(sc); }

    /* functions for inspecting and converting from/to scheme objects */
    template <typename T>
    bool is(s7_pointer p)
    {
        if constexpr(std::is_same_v<T, s7_pointer>) {
            return p;
        } else if constexpr(std::is_same_v<T, bool>) {
            return s7_is_boolean(p);
        } else if constexpr(std::is_same_v<T, s7_int>
                  || std::is_same_v<T, int>
                  || std::is_same_v<T, short>
                  || std::is_same_v<T, long>) {
            return s7_is_integer(p);
        } else if constexpr(std::is_same_v<T, double>
                         || std::is_same_v<T, float>) {
            return s7_is_real(p);
        } else if constexpr(std::is_same_v<T, const char *>
                         || std::is_same_v<T, std::string>
                         || std::is_same_v<T, std::string_view>) {
            return s7_is_string(p);
        } else if constexpr(std::is_same_v<T, unsigned char>) {
            return s7_is_character(p);
        } else if constexpr(std::is_same_v<T, std::span<s7_pointer>>) {
            return s7_is_vector(p);
        } else if constexpr(std::is_same_v<T, std::span<int>>) {
            return s7_is_int_vector(p);
        } else if constexpr(std::is_same_v<T, std::span<double>>) {
            return s7_is_float_vector(p);
        } else if constexpr(std::is_same_v<T, std::span<uint8_t>>) {
            return s7_is_byte_vector(p);
        } else if constexpr(std::is_null_pointer_v<T>) {
            return s7_is_null(sc, p);
        } else if constexpr(std::is_pointer_v<T>) {
            return s7_is_c_pointer(p);
        } else if constexpr(std::is_same_v<T, List>) {
            return s7_is_pair(p);
        }
        // TODO: add is_rational, is_ratio and is_complex?
        return s7_is_c_object(p) && s7_c_object_type(p) == TypeTag<T>::tag;
    }

    template <typename T>
    s7_pointer from(const T &x)
    {
        if constexpr(std::is_same_v<T, s7_pointer>) { return x; }
        else if constexpr(std::is_same_v<T, bool>) {
            return s7_make_boolean(sc, x);
        } else if constexpr(std::is_same_v<T, s7_int>
                       || std::is_same_v<T, int>
                       || std::is_same_v<T, short>
                       || std::is_same_v<T, long>) {
            return s7_make_integer(sc, x);
        } else if constexpr(std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return s7_make_real(sc, x);
        } else if constexpr(std::is_same_v<T, const char *>) {
            return s7_make_string(sc, x);
        } else if constexpr(std::is_same_v<T, std::string>) {
            return s7_make_string_with_length(sc, x.c_str(), x.size());
        } else if constexpr(std::is_same_v<T, std::string_view>) {
            return s7_make_string_with_length(sc, x.data(), x.size());
        } else if constexpr(std::is_same_v<T, unsigned char>) {
            return s7_make_character(sc, x);
        }  else if constexpr(std::is_same_v<T, std::span<double>>) {
            auto vec = s7_make_vector(sc, x.size(), 1, nullptr);
            for (size_t i = 0; i < x.size(); i++) {
                s7_vector_set(sc, vec, i, x[i]);
            }
            return vec;
        } else if constexpr(std::is_same_v<T, std::span<int>>) {
            auto vec = s7_make_int_vector(sc, x.size(), 1, nullptr);
            for (size_t i = 0; i < x.size(); i++) {
                s7_int_vector_set(vec, i, x[i]);
            }
            return vec;
        } else if constexpr(std::is_same_v<T, std::span<double>>) {
            auto vec = s7_make_float_vector(sc, x.size(), 1, nullptr);
            for (size_t i = 0; i < x.size(); i++) {
                s7_float_vector_set(vec, i, x[i]);
            }
            return vec;
        } else if constexpr(std::is_same_v<T, std::span<uint8_t>>) {
            auto vec = s7_make_byte_vector(sc, x.size(), 1, nullptr);
            for (size_t i = 0; i < x.size(); i++) {
                s7_byte_vector_set(vec, i, x[i]);
            }
            return vec;
        } else if constexpr(std::is_null_pointer_v<T>) {
            return s7_nil(sc);
        } else if constexpr(std::is_pointer_v<T>) {
            return s7_make_c_pointer(sc, x);
        } else if constexpr(std::is_same_v<T, List>) {
            return x.ptr();
        }
        if (TypeTag<T>::tag != -1) {
            return s7_make_c_object(sc, TypeTag<T>::tag, reinterpret_cast<void *>(new T(x)));
        }
        assert(false && "failed to create s7_pointer from T");
    }

    template <typename T>
    auto to(s7_pointer p)
    {
        assert(is<T>(p) && "p isn't an object of type T");
        if constexpr(std::is_same_v<T, s7_pointer>) {
            return p;
        } else if constexpr(std::is_same_v<T, bool>) {
            return s7_boolean(sc, p);
        } else if constexpr(std::is_same_v<T, s7_int>
                         || std::is_same_v<T, int>
                         || std::is_same_v<T, short>
                         || std::is_same_v<T, long>) {
            return s7_integer(p);
        } else if constexpr(std::is_same_v<T, double>) {
            return s7_real(p);
        } else if constexpr(std::is_same_v<T, const char *>) {
            return s7_string(p);
        } else if constexpr(std::is_same_v<T, std::string>) {
            return std::string(s7_string(p));
        } else if constexpr(std::is_same_v<T, std::string_view>) {
            return std::string_view(s7_string(p));
        } else if constexpr(std::is_same_v<T, unsigned char>) {
            return s7_character(p);
        } else if constexpr(std::is_same_v<T, std::span<s7_pointer>>) {
            return std::span(s7_vector_elements(p), s7_vector_length(p));
        } else if constexpr(std::is_same_v<T, std::span<s7_int>>) {
            return std::span(s7_int_vector_elements(p), s7_vector_length(p));
        } else if constexpr(std::is_same_v<T, std::span<double>>) {
            return std::span(s7_float_vector_elements(p), s7_vector_length(p));
        } else if constexpr(std::is_same_v<T, std::span<uint8_t>>) {
            return std::span(s7_byte_vector_elements(p), s7_vector_length(p));
        } else if constexpr(std::is_null_pointer_v<T>) {
            return nullptr;
        } else if constexpr(std::is_same_v<T, std::is_pointer<T>>) {
            return s7_c_pointer(p);
        } else if constexpr(std::is_same_v<T, List>) {
            return List(p);
        } else {
            return reinterpret_cast<T *>(s7_c_object_value(p));
        }
    }

    template <typename T>
    auto to_opt(s7_pointer p) -> std::optional<decltype(std::declval<s7>().to<T>(p))>
    {
        if (!is<T>(p)) {
            return std::nullopt;
        }
        return std::optional(to<T>(p));
    }

    template <typename T = std::string>
    T to_string(s7_pointer p)
    {
        return this->to<T>(s7_object_to_string(sc, p, true));
    }

    template <typename T>
    List list(const T &arg)
    {
        return List(s7_cons(sc, this->from<T>(arg), s7_nil(sc)));
    }

    template <typename T, typename... Args>
    List list(const T &arg, Args&&... args)
    {
        return List(s7_cons(sc, this->from<T>(arg), list(args...).ptr()));
    }

    /* errors */
    s7_pointer wrong_argument_type_error(std::string_view function_name, int arg_number, s7_pointer arg, std::string_view desc)
    {
        return s7_wrong_type_arg_error(sc, function_name.data(), arg_number, arg, desc.data());
    }

    /* variables and symbols */
    template <typename T>
    s7_pointer define(std::string_view name, const T &value, std::string_view doc = "")
    {
        auto object = this->from<T>(value);
        return s7_define_variable_with_documentation(sc, name.data(), object, doc.data());
    }

    template <typename T>
    s7_pointer define_const(std::string_view name, const T &value, std::string_view doc = "")
    {
        auto object = this->from<T>(value);
        return s7_define_constant_with_documentation(sc, name.data(), object, doc.data());
    }

    friend struct Variable;

    Variable operator[](std::string_view name);

    /* functions */
    template <typename... T>
    s7_pointer call(std::string_view name, T&&... args)
    {
        return s7_call(sc, s7_name_to_value(sc, name.data()), this->list(args...).ptr());
    }

    s7_pointer define_function(std::string_view name, s7_function func, s7_int required_args, s7_int optional_args, bool rest_arg, std::string_view doc = "")
    {
        return s7_define_function(sc, name.data(), func, required_args, optional_args, rest_arg, doc.data());
    }

    template <typename R, typename... Args>
    s7_pointer define_fun_from_ptr(std::string_view name, std::string_view doc, R (*fptr)(Args...))
    {
        constexpr auto num_args = sizeof...(Args);

        auto let = s7_sublet(this->sc, s7_curlet(this->sc), this->nil());
        s7_define(sc, let, s7_make_symbol(sc, "test-var"), this->from(42));

        auto f = [](s7_scheme *sc, s7_pointer args) -> s7_pointer {
            auto let = s7_curlet(sc);
            auto test_var = s7_let_ref(sc, let, s7_make_symbol(sc, "test-var"));
            printf("test_var = %d\n", s7_integer(test_var)); // doesn't quite work...
            return s7_nil(sc);
        };

        auto p = s7_make_typed_function_with_environment(
            this->sc, name.data(), f, num_args, 0, false, doc.data(),
            // TODO: temporary signature
            s7_make_signature(sc, 4, s7_t(sc), s7_t(sc), s7_make_symbol(sc, "real?"), s7_make_symbol(sc, "real?")),
            let
        );

        s7_define(sc, s7_rootlet(sc), s7_make_symbol(sc, name.data()), p);
        return p;
    }

    /* usertypes */
    template <typename T>
    void make_c_type(std::string_view name)
    {
        auto tag = s7_make_c_type(sc, name.data());
        TypeTag<T>::tag = tag;

        if constexpr(requires { T(); }) {
            auto ctor = [](s7_scheme *sc, s7_pointer args) -> s7_pointer {
                s7 *scheme = reinterpret_cast<s7 *>(&sc);
                return s7_make_c_object(sc, TypeTag<T>::tag, reinterpret_cast<void *>(new T(sc)));
            };
            auto ctor_name = std::string("make-") + std::string(name);
            auto doc = std::string("(") + ctor_name + ") creates a new " + std::string(name);
            this->define_function(ctor_name.c_str(), ctor, 0, 0, false, doc.c_str());
        }

        if constexpr(requires(T t) { t.gc_mark(*this); }) {
            s7_c_type_set_gc_mark(sc, tag, [](s7_scheme *sc, s7_pointer obj) -> s7_pointer {
                s7 *scheme = reinterpret_cast<s7 *>(&sc);
                T *o = reinterpret_cast<T *>(obj);
                o->gc_mark(*scheme);
                return nullptr;
            });
        }

        if constexpr(requires(T t) { t.gc_free(*this); }) {
            s7_c_type_set_gc_free(sc, tag, [](s7_scheme *sc, s7_pointer obj) -> s7_pointer {
                s7 *scheme = reinterpret_cast<s7 *>(&sc);
                T *o = reinterpret_cast<T *>(obj);
                o->gc_free(*scheme);
                delete o;
                return nullptr;
            });
        } else {
            s7_c_type_set_gc_free(sc, tag, [](s7_scheme *sc, s7_pointer obj) -> s7_pointer {
                T *o = reinterpret_cast<T *>(obj);
                delete o;
                return nullptr;
            });
        }

        if constexpr(requires(T a, T b) { a == b; }) {
            s7_c_type_set_is_equal(sc, tag, [](s7_scheme *sc, s7_pointer args) -> s7_pointer {
                s7_pointer a = s7_car(args);
                s7_pointer b = s7_cadr(args);
                if (a == b) {
                    return s7_t(sc);
                }
                auto tag = TypeTag<T>::tag;
                if (!s7_is_c_object(b) || (s7_c_object_type(b) != tag)) {
                    return s7_f(sc);
                }
                T *pa = (T *) s7_c_object_value(a);
                T *pb = (T *) s7_c_object_value(b);
                return s7_make_boolean(sc, *pa == *pb);
            });
        }

        if constexpr(requires(T t) { t.to_string(*this); }) {
            s7_c_type_set_to_string(sc, tag, [](s7_scheme *sc, s7_pointer args) -> s7_pointer {
                s7 *scheme = reinterpret_cast<s7 *>(&sc);
                return s7_make_string(sc, reinterpret_cast<T *>(s7_c_object_value(s7_car(args)))->to_string(*scheme).c_str());
            });
        }

        auto is_name = std::string(name) + "?";
        auto is_doc  = std::string("(") + is_name + " x) checks if x is a " + std::string(name);
        auto is = [](s7_scheme *sc, s7_pointer args) {
            auto &scheme = *reinterpret_cast<s7 *>(&sc);
            auto tag = TypeTag<T>::tag;
            return scheme.from<bool>(scheme.is<T>(s7_car(args)));
        };
        this->define_function(is_name.c_str(), is, 0, 0, false, is_doc.c_str());
    }

    std::optional<size_t> type_tag_of(std::string_view name)
    {
        auto types = List(this->eval("(*s7* 'c-types)"));
        size_t i = 0;
        for (const auto &p : types) {
            if (name == to<std::string_view>(p)) {
                return i;
            }
            i++;
        }
        return std::nullopt;
    }
};

struct Variable {
    s7 *scheme;
    s7_pointer sym;

    template <typename T>
    Variable & operator=(const T &v)
    {
        s7_symbol_set_value(scheme->sc, sym, scheme->from<T>(v)); return *this;
    }

    template <typename T>
    T as()
    {
        return scheme->to<T>(s7_symbol_value(scheme->sc, sym));
    }

    template <typename T>
    auto as_opt()
    {
        return scheme->to_opt<T>(s7_symbol_value(scheme->sc, sym));
    }
};

Variable s7::operator[](std::string_view name)
{
    auto sym = s7_symbol_table_find_name(this->sc, name.data());
    if (!sym) {
        sym = s7_define_variable(this->sc, name.data(), s7_nil(sc));
    }
    return Variable(this, sym);
}

} // namespace s7
