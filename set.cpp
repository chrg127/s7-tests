#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unordered_set>
#include <algorithm>
#include "s7/s7.h"

int tag = 0;

struct S7Equal {
    s7_scheme *sc;

    bool operator()(const s7_pointer &a, const s7_pointer &b) const {
        return s7_is_equal(sc, a, b);
    }
};

struct S7Hash {
    s7_scheme *sc;

    size_t operator()(const s7_pointer& p) const
    {
        return s7_hash_code(sc, p, s7_name_to_value(sc, "equal?"));
    }
};

struct Set {
    std::unordered_set<s7_pointer, S7Hash, S7Equal> set;
};

s7_pointer make_set(s7_scheme *sc, s7_pointer args)
{
    Set *set = new Set();
    set->set = std::unordered_set<s7_pointer, S7Hash, S7Equal>(512, S7Hash(sc), S7Equal(sc));
    return s7_make_c_object(sc, tag, (void *) set);
}

s7_pointer is_set(s7_scheme *sc, s7_pointer args)
{
    return(s7_make_boolean(sc, s7_is_c_object(s7_car(args))
                            && s7_c_object_type(s7_car(args)) == tag));
}

template <typename T>
bool is_subset(const T &sa, const T &sb)
{
    return std::all_of(sb.begin(), sb.end(), [&](const auto &x) {
        return sa.find(x) != sa.end();
    });
}

template <typename T>
bool sets_are_equal(const T &sa, const T &sb)
{
    return sa.size() == sb.size() && is_subset(sa, sb);
}

s7_pointer set_is_eq(s7_scheme *sc, s7_pointer args)
{
    s7_pointer p1 = s7_car(args);
    s7_pointer p2 = s7_cadr(args);
    if (p1 == p2) {
        return s7_t(sc);
    }
    if (!s7_is_c_object(p2) || (s7_c_object_type(p2) != tag)) {
        return s7_f(sc);
    }
    Set *s1 = (Set *) s7_c_object_value(p1);
    Set *s2 = (Set *) s7_c_object_value(p2);
    return s7_make_boolean(sc, sets_are_equal(s1->set, s2->set));
}

s7_pointer set_to_string(s7_scheme *sc, s7_pointer args)
{
    Set *set = (Set *) s7_c_object_value(s7_car(args));
    std::string str = "<#set(";
    for (const s7_pointer &value : set->set) {
        str += std::string(s7_object_to_c_string(sc, value)) + ", ";
    }
    str += ")>";
    return s7_make_string(sc, str.c_str());
}

s7_pointer mark_set(s7_scheme *sc, s7_pointer obj)
{
    Set *set = (Set *) s7_c_object_value(obj);
    for (s7_pointer value : set->set) {
        s7_mark(value);
    }
    return NULL;
}

s7_pointer free_set(s7_scheme *sc, s7_pointer obj)
{
    delete ((Set *) s7_c_object_value(obj));
    return NULL;
}

s7_pointer set_add(s7_scheme *sc, s7_pointer args)
{
    Set *set = (Set *) s7_c_object_value(s7_car(args));
    s7_pointer value = s7_cadr(args);
    set->set.insert(value);
    return s7_undefined(sc);
}

int main(int argc, char **argv)
{
    s7_scheme *s7 = s7_init();
    s7_define_variable(s7, "*listener-prompt*", s7_make_string(s7, ">"));

    tag = s7_make_c_type(s7, "set");
    s7_c_type_set_gc_free(s7, tag, free_set);
    s7_c_type_set_gc_mark(s7, tag, mark_set);
    s7_c_type_set_is_equal(s7, tag, set_is_eq);
    s7_c_type_set_to_string(s7, tag, set_to_string);
    s7_define_function(s7, "set", make_set, 0, 0, false, "(make-set) creates a new set");
    s7_define_function(s7, "set?", is_set, 1, 0, false, "(set? anything) returns #t if its argument is a set");
    s7_define_function(s7, "set-add!", set_add, 2, 0, false, "(set-add! set value) adds value to set");

    while (1) {
        char buffer[512];
        printf("> ");
        fgets(buffer, 512, stdin);
        if ((buffer[0] != '\n') || (strlen(buffer) > 1)) {
            char response[1024];
            snprintf(response, 1024, "(write %s)", buffer);
            s7_eval_c_string(s7, response); /* evaluate input and write the result */
        }
        printf("\n");
    }
}
