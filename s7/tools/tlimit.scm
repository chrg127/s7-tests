;; timing test for multi-parameter funcs and so on

(define size 10000) ; tried also 100k and 1M -- most the time is in format_to_port
;;; 10k:  0.109u 0.056s
;;; 100k: 1.762u 0.259s
;;; 1M:  54.048u 3.094s

(define tmp-output-file "too-many-lets.scm")
(define time-column 32)

(define (report-time name value)
  ;; (format *stderr* "~D ~A output: ~NT~,4G~%" size name time-column (- (*s7* 'cpu-time) start))
  ;; (set! start (*s7* 'cpu-time))

  (unless (equal? (load tmp-output-file) value)
    (format *stderr* "~D ~A: ~S~%" size name (load tmp-output-file)))

  (format *stderr* "~D ~A: ~NT~,4G~%" size name time-column (- (*s7* 'cpu-time) start))
  (set! start (*s7* 'cpu-time)))
  

(define start (*s7* 'cpu-time))


;; -------- N let vars --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~% (+ f_0 f_~D))~%" (- size 1))))

(report-time "let vars" 2)


;; -------- N let* vars --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let* (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~% (+ f_0 f_~D))~%" (- size 1))))

(report-time "let* vars" 2)


;; -------- N letrec vars --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(letrec (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~% (+ f_0 f_~D))~%" (- size 1))))

(report-time "letrec vars" 2)


;; -------- N letrec* vars --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(letrec* (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~% (+ f_0 f_~D))~%" (- size 1))))

(report-time "letrec* vars" 2)


;; -------- N do vars --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(do (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1 (+ f_~D 1))~%" i i))
    (format p "   ) ((= f_0 1) (+ f_0 f_~D)))~%" (- size 1))))

(report-time "do vars" 2)


;; -------- N let vars added --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let vars added" size)


;; -------- N let* vars added --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let* (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let* vars added" size)


;; -------- N let vars = --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (= ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let vars =" #t)


;; -------- N let vars max --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (max ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let vars max" 1)


;; -------- N let vars or --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D #f)~%" i))
    (format p "   )~%  (or ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let vars or" #f)


;; -------- N let vars add values1 --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (+ (values ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")))~%")))

(report-time "let vars add values1" size)


;; -------- N let vars add values2 --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(+ (let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (values ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")))~%")))

(report-time "let vars add values2" size)


;; -------- N let vars nested add --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D (+ " i))
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p ")"))
    (format p "))~%")))

(report-time "let vars nested add" size)


;; -------- N let vars lambda add --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%  ((lambda args (apply + args)) ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let vars lambda add" size)


;; -------- N inlet vars add --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(with-let (inlet ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "'f_~D 1 " i))
    (format p "   )~%  (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "inlet vars add" size)


;; -------- N let vars string --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D #\\a)~%" i))
    (format p "   )~%  (string ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))~%")))

(report-time "let vars string" (make-string size #\a))


;; -------- N let vars mapped --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D -1)~%" i))
    (format p "   )~% (car (map symbol? '(")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))))~%")))

(report-time "let vars mapped" #t)


;; -------- N let vars sorted --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D ~D)~%" i i))
    (format p "   )~% (vector-ref (sort! (vector ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ") <) 0))~%")))

(report-time "let vars sorted" 0)


;; -------- call/cc return values --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(+ (call/cc (lambda (return) (return ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "~D " i))
    (format p "))))")))

(report-time "call/cc return values" (* (/ size 2) (- size 1)))


;; -------- lambda args --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "((lambda (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ") (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")) ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "~D " i))
    (format p ")")))

(report-time "lambda args" (* (/ size 2) (- size 1)))


;; -------- lambda* args --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "((lambda* (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ") (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")) ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "~D " i))
    (format p ")")))

(report-time "lambda* args" (* (/ size 2) (- size 1)))


;; -------- lambda* args + defaults --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "((lambda* (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "(f_~D ~D) " i i))
    (format p ") (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")))")))

(report-time "lambda* args + defaults" (* (/ size 2) (- size 1)))


;; -------- named let args --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let loop (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "(f_~D ~D) " i i))
    (format p ") (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")) ")))

(report-time "named let args" (* (/ size 2) (- size 1)))


;; -------- named let* args --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let* loop (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "(f_~D ~D) " i i))
    (format p ") (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")) ")))

(report-time "named let* args" (* (/ size 2) (- size 1)))


;; -------- let-temporarily args --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "(f_~D 0)" i))
    (format p ") (let-temporarily (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "(f_~D ~D)" i i))
    (format p ") (+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p ")))")))

(report-time "let-temporarily args" (* (/ size 2) (- size 1)))


;; -------- memq --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(memq 'f_~D '(" (- size 1)) ; not ints here -- not eq?!
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))")))

(report-time "memq" (list (symbol "f_" (number->string (- size 1)))))


;; -------- assq --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(assq 'f_~D '(" (- size 1))
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "(f_~D ~D) " i (* i 2)))
    (format p "))")))

(report-time "assq" (list (symbol "f_" (number->string (- size 1))) (* 2 (- size 1))))


;; -------- read-time vector --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(apply + (vector->list #(")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "~D " i))
    (format p ")))")))

(report-time "read-time vector" (* (/ size 2) (- size 1)))


;; -------- N lets nested --------
(let-temporarily ((size (min size (min size 1000))))
  (call-with-output-file tmp-output-file
    (lambda (p)
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p "(let ((f_~D ~D)) " i i))
      (format p "(+ ")
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p "f_~D " i))
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p ")"))
      (format p ")")))
  
  (report-time "lets nested" (* (/ size 2) (- size 1))))


;; -------- N let vars+set+add --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D ~D)~%" i i))
    (format p "   )~%")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (set! f_~D (* 2 f_~D))~%" i i))
    (format p "(+ ")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "f_~D " i))
    (format p "))")))

(report-time "let vars+set+add" (* size (- size 1)))


;; -------- N lambdas nested --------
(let-temporarily ((size (min size (min size 1000))))
  (call-with-output-file tmp-output-file
    (lambda (p)
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p "("))
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p "(lambda (f_~D)~%" i))
      (format p "(+ ")
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p "f_~D " i))
      (do ((i 0 (+ i 1)))
	  ((= i (+ size 1)))
	(format p ")"))
      (do ((i 0 (+ i 1)))
	  ((= i size))
	(format p " ~D)" i))))
  
  (report-time "lambdas nested" (* (/ size 2) (- size 1))))


;; these are aimed at the optimizer
;; -------- N vars + 1-arg func --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(define (f) (let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%")
    (do ((i 0 (+ i 1)))
	((not (< i size)))
      (format p "(+ (abs f_~D) (abs f_~D))~%" i (- size i 1)))
    (format p ")) (f)~%")))

(report-time "vars + 1-arg func" 2)


;; -------- N vars + 2-arg func --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(define (f) (let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%")
    (do ((i 0 (+ i 1)))
	((not (< i size)))
      (format p "(ash f_~D f_~D)~%" i (- size i 1)))
    (format p ")) (f)~%")))

(report-time "vars + 2-arg func" 2)


;; -------- N vars + 3-arg func --------
(call-with-output-file tmp-output-file
  (lambda (p)
    (format p "(define (f) (let (")
    (do ((i 0 (+ i 1)))
	((= i size))
      (format p "  (f_~D 1)~%" i))
    (format p "   )~%")
    (do ((i 0 (+ i 1)))
	((not (< i size)))
      (format p "(+ f_~D f_~D 1)~%" i (- size i 1)))
    (format p ")) (f)~%")))

(report-time "vars + 3-arg func" 3)


(delete-file tmp-output-file)
