;;; reset names: back-to-v-10

(define file-names '(("cb.scm" . "v-cb")
		     ("concordance.scm" . "v-str")
		     ("dup.scm" . "v-dup")
		     ("fbench.scm" . "v-fb")
		     ("full-snd-test.scm" . "v-sg")
		     ("lt.scm" . "v-lt")
		     ("make-index.scm" . "v-index")
		     ("s7test.scm" . "v-test")
		     ("snd-test.scm" . "v-call")
		     ("tall.scm" . "v-all")
		     ("tari.scm" . "v-ari")
		     ("tauto.scm" . "v-auto")
		     ("tbig.scm" . "v-big")
		     ("tcase.scm" . "v-case")
		     ("tclo.scm" . "v-clo")
		     ("tcomplex.scm" . "v-complex")
		     ("tcopy.scm" . "v-cop")
		     ("teq.scm" . "v-eq")
		     ("test-all.scm" . "v-b")
		     ("texit.scm" . "v-exit")
		     ("tfft.scm" . "v-fft")
		     ("tform.scm" . "v-form")
		     ("tgc.scm" . "v-gc")
		     ("tgen.scm" . "v-gen")
		     ("tgsl.scm" . "v-gsl")
		     ("thash.scm" . "v-hash")
		     ("thook.scm" . "v-hook")
		     ("timp.scm" . "v-imp")
		     ("tio.scm" . "v-io")
		     ("titer.scm" . "v-iter")
		     ("tlamb.scm" . "v-lamb")
		     ("tleft.scm" . "v-left")
		     ("tlet.scm" . "v-let")
		     ("tlimit.scm" . "v-limit")
		     ("tlist.scm" . "v-list")
		     ("tload.scm" . "v-load")
		     ("tmac.scm" . "v-mac")
		     ("tmap-hash.scm" . "v-map-hash")
		     ("tmap.scm" . "v-map")
		     ("tmat.scm" . "v-mat")
		     ("tmisc.scm" . "v-misc")
		     ("tmock.scm" . "v-mock")
		     ("tmv.scm" . "v-mv")
		     ("tnum.scm" . "v-num")
		     ("tobj.scm" . "v-obj")
		     ("tpeak.scm" . "v-peak")
		     ("trclo.scm" . "v-rclo")
		     ("tread.scm" . "v-read")
		     ("trec.scm" . "v-rec")
		     ("tref.scm" . "v-ref")
		     ("tset.scm" . "v-set")
		     ("tshoot.scm" . "v-shoot")
		     ("tsort.scm" . "v-sort")
		     ("tstar.scm" . "v-star")
		     ("tvect.scm" . "v-vect")
		     ))

(define (last-callg)
  (let ((name (system "ls callg*" #t)))
    (let ((len (length name)))
      (do ((i 0 (+ i 1)))
	  ((or (= i len)
	       (char-whitespace? (name i)))
	   (substring name 0 i))))))

(define (next-file f)
  (let ((name (system (format #f "ls -t ~A*" f) #t)))
    (let ((len (length name)))
      (do ((i 0 (+ i 1)))
	  ((or (= i len)
	       (and (char-numeric? (name i))
		    (char-numeric? (name (+ i 1)))))
	   (+ 1 (string->number (substring name i (+ i 2)))))))))

(define (call-valgrind)
  (for-each
   (lambda (caller+file)
     (system "rm callg*")
     (format *stderr* "~%~NC~%~NC ~A ~NC~%~NC~%" 40 #\- 16 #\- (cadr caller+file) 16 #\- 40 #\-)
     (system (format #f "valgrind --tool=callgrind ./~A ~A" (car caller+file) (cadr caller+file)))

     (let ((outfile (cdr (assoc (cadr caller+file) file-names))))
       (let ((next (next-file outfile)))
	 (system (format #f "callgrind_annotate --auto=yes --show-percs=no --threshold=100 ~A > ~A~D" (last-callg) outfile next))
	 (format *stderr* "~NC ~A~D -> ~A~D: ~NC~%" 8 #\space outfile (- next 1) outfile next 8 #\space)
	 (system (format #f "./snd compare-calls.scm -e '(compare-calls \"~A~D\" \"~A~D\")'" outfile (- next 1) outfile next)))))

   (list (list "repl" "tpeak.scm")
	 (list "repl" "tref.scm")
	 (list "repl" "tlimit.scm")
	 (list "snd -noinit" "make-index.scm")
	 (list "repl" "tmock.scm")
	 (list "repl" "tvect.scm")
	 (list "repl" "thook.scm")
	 (list "repl" "texit.scm")
	 (list "repl" "tauto.scm")
	 (list "repl" "s7test.scm")
	 (list "repl" "lt.scm")
	 (list "repl" "dup.scm")
	 (list "repl" "tread.scm")
	 (list "repl" "tcopy.scm")
	 (list "repl" "trclo.scm")
	 (list "repl" "tload.scm")
	 (list "repl" "tmat.scm")
	 (list "repl" "fbench.scm")
	 (list "repl" "tsort.scm")
	 (list "repl" "titer.scm")
	 (list "repl" "tio.scm")
	 (list "repl" "tobj.scm")
	 (list "repl" "teq.scm")
	 (list "repl" "tmac.scm")
	 (list "repl" "tcomplex.scm")
	 (list "repl" "tcase.scm")
	 (list "repl" "tmap.scm")
	 (list "repl" "tlet.scm")
	 (list "repl" "tfft.scm")
	 (list "repl" "tshoot.scm")
	 (list "repl" "tstar.scm")
	 (list "repl" "tform.scm")
	 (list "repl" "concordance.scm")
	 (list "repl" "tnum.scm")
	 (list "repl" "tlist.scm")
	 (list "repl" "trec.scm")
	 (reader-cond ((not (provided? 'gmp)) (list "repl" "tari.scm")))
	 (list "repl" "tgsl.scm")
	 (list "repl" "tset.scm")
	 (list "repl" "tleft.scm")
	 (list "repl" "tmisc.scm")
	 (list "repl" "tclo.scm")
	 (list "repl" "tlamb.scm")
	 (list "repl" "tgc.scm")
	 (list "repl" "thash.scm")
	 (list "repl" "cb.scm")
	 (list "repl" "tmap-hash.scm")
	 (list "snd -noinit" "tgen.scm")    ; repl here + cload sndlib was slower
	 (list "snd -noinit" "tall.scm")
	 (list "repl" "timp.scm")
	 (list "repl" "tmv.scm")
	 (list "snd -l" "snd-test.scm")
	 (list "snd -l" "full-snd-test.scm")
	 (list "repl" "tbig.scm")
	 )))

(call-valgrind)

(when (file-exists? "test.table")
  (system "mv test.table old-test.table"))
(load "compare-calls.scm")
(combine-latest)

(exit)