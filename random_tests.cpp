#include <cstdio>
#include <cstdlib>
#include "s7cpp.cpp"

void scheme_defined_function()
{
    s7 s7;
    int v = 1;
    s7.defvar("an-integer", v);
    s7.eval("(define (add1 a) (+ a 1))");
    printf("%s\n", std::format("an-integer: {}\n", s7.get_value<int>("an-integer")).c_str());
    int new_value = 32;
    s7.set_value("an-integer", new_value);
    printf("%s\n", std::format("an-integer: {}\n", s7.get_value<int>("an-integer")).c_str());
    int res = s7.to<int>(s7.call("add1", 2));
    printf("%s\n", std::format("(add1 2): {}\n", res).c_str());
}

s7_pointer add1(s7_scheme *sc, s7_pointer args)
{
    printf("add1 called\n");
    if (s7_is_integer(s7_car(args))) {
        return s7_make_integer(sc, s7_integer(s7_car(args)) + 1);
    }
    return s7_wrong_type_arg_error(sc, "add1", 1, s7_car(args), "an integer");
}

auto defstruct_macro = R"(
	(define-macro (defstruct struct-name . fields)
	  (let* ((name (if (list? struct-name) (car struct-name) struct-name))
		 (sname (if (string? name) name (symbol->string name)))

		 (fsname (if (list? struct-name)
			     (let ((cname (assoc :conc-name (cdr struct-name))))
			       (if cname
				   (symbol->string (cadr cname))
				   sname))
			     sname))

		 (make-name (if (list? struct-name)
				(let ((cname (assoc :constructor (cdr struct-name))))
				  (if cname
				      (cadr cname)
				      (symbol "make-" sname)))
				(symbol "make-" sname)))

		 (copy-name (if (list? struct-name)
				(let ((cname (assoc :copier (cdr struct-name))))
				  (if cname
				      (cadr cname)
				      (symbol "copy-" sname)))
				(symbol "copy-" sname)))

		 (field-names (map (lambda (n)
				     (symbol->string (if (list? n) (car n) n)))
				   fields))

		 (field-types (map (lambda (field)
				     (if (list? field)
					 (apply (lambda* (val type read-only) type) (cdr field))
					 #f))
				   fields))

		 (field-read-onlys (map (lambda (field)
					  (if (list? field)
					      (apply (lambda* (val type read-only) read-only) (cdr field))
					      #f))
					fields)))
	    `(begin

	       (define ,(symbol sname "?")
		 (lambda (obj)
		   (and (vector? obj)
			(eq? (obj 0) ',(string->symbol sname)))))

	       (define* (,make-name
			 ,@(map (lambda (n)
				  (if (and (list? n)
					   (>= (length n) 2))
				      (list (car n) (cadr n))
				      (list n #f)))
				fields))
		 (vector ',(string->symbol sname) ,@(map string->symbol field-names)))

	       (define ,copy-name copy)

	       ,@(map (let ((ctr 1))
			(lambda (n type read-only)
			  (let ((val (if read-only
					 `(define ,(symbol fsname "-" n)
					    (lambda (arg) (arg ,ctr)))
					 `(define ,(symbol fsname "-" n)
					    (dilambda
					     (lambda (arg) (arg ,ctr))
					     (lambda (arg val) (set! (arg ,ctr) val)))))))
			    (set! ctr (+ 1 ctr))
			    val)))
		      field-names field-types field-read-onlys))))
)";

void c_defined_function()
{
    s7 s7;
    s7.define_function("add1", add1, 1, 0, false, "(add1 int): adds 1 to int");
    s7.defvar("my-pi", 3.14159265);
    for (;;) {
        char buffer[512];
        fprintf(stdout, "\n> ");
        fgets(buffer, 512, stdin);
        if (buffer[0] != '\n' || strlen(buffer) > 1) { 
            s7.eval(std::format("(write {})", buffer));
        }
    }
}

void conversion_test()
{
    s7 s7;
    s7.defvar("a-list", s7.mklist(1, 2, 3));
}

void repl()
{
    s7 s7;
    s7.eval(defstruct_macro);
    for (;;) {
        char buffer[512];
        fprintf(stdout, "\n> ");
        fgets(buffer, 512, stdin);
        printf("got %s\n", buffer);
        if (buffer[0] != '\n' || strlen(buffer) > 1) { 
            s7.eval(std::format("(write {})", buffer));
        }
    }
}

double add_double(double a, double b) { return a + b; }

int main()
{
    s7 s7;
    s7.define_fun_from_ptr("hi", "doc", add_double);
    s7.define_fun_from_ptr("repl", "doc", repl);
}

