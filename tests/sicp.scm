; A scheme evaluator & a register-machine simulator & a scheme compiler
;
; Copyright (C) 2021 Tongjie Liu <tongjieandliu@gmail.com>.
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.

; Table of Contents
; SECTION 1: Basic Tools
; SECTION 2: The Explicit-Control Evaluator
; SECTION 3: The Register-Machine Simulator
; SECTION 4: The Compiler
; SECTION 5: Syntax Procedures
; SECTION 6: Examples
;
;
; Interfaces
;     Evaluator:     1. (make-eceval)
;                    2. (eceval-start)
;                    3. (eceval-debug)
;
;     Simulator:     1. (make-machine op-table controller-text)
;                    2. (start machine)
;                    3. (get-register-contents machine reg-name)
;                    4. (set-register-contents! machine reg-name new-contents)
;                    5. (machine-get-instruction-count machine)
;                    6. (machine-reset-instruction-count! machine)
;                    7. (machine-trace-on machine)
;                    8. (machine-trace-off machine)
;                    9. (machine-trace-register-on machine reg-name)
;                   10. (machine-trace-register-off machine reg-name)
;                   11. (set-breakpoint machine breakpoint-name line-num)
;                   12. (proceed-machine machine)
;                   13. (cancel-breakpoint machine breakpoint-name line-num)
;                   14. (cancel-all-breakpoints machine)
;
;     Compiler:      1. (compile exp target linkage ct-env)
;                    2. (compiler-run exp)
;                    3. (compiler-print exp target linkage)
;                    4. (compiler-start)




(include "seq")




; --- SECTION 1: Basic Tools ---
(define (assoc key records)
    (cond ((null? records) #f)
	  ((equal? key (caar records))
	      (car records))
	  (else (assoc key (cdr records)))))



(define (tagged-list? alist type-tag)
    (and (pair? alist)
	 (let ((head (car alist)))
	     (and (symbol? head)
	          (eq? head type-tag)))))


(define (adjoin alist item)
    (append alist (list item)))


(define (member item alist)
    (cond ((null? alist) #f)
	  ((equal? item (car alist)) #t)
	  (else (member item (cdr alist)))))


(define (memq item alist)
    (cond ((null? alist) #f)
	  ((eq? item (car alist)) item)
	  (else (memq item (cdr alist)))))


(define (list-union a b)
    (cond ((null? a) b)
	  ((memq (car a) b)
	      (list-union (cdr a) b))
	  (else (cons (car a)
		      (list-union (cdr a) b)))))


(define (list-difference a b)
    (cond ((null? a) '())
	  ((memq (car a) b)
	      (list-difference (cdr a) b))
	  (else (cons (car a)
		      (list-difference (cdr a) b)))))




; --- SECTION 2: The Explicit-Control Evaluator ---
(define LOAD-PREDEFINED-PROCEDURES
    (list (list "tests"
                '(begin (define (tagged-list? alist type-tag)
                            (and (pair? alist)
	                         (let ((head (car alist)))
	                             (and (symbol? head)
	                                  (eq? head type-tag)))))

                        (define yes '(yes 1))
			(define no '(no a b c d e))

                        (define (assoc key records)
                            (cond ((null? records) #f)
	                          ((equal? key (car (car records)))
	                              (car records))
	                          (else (assoc key (cdr records)))))

			(define test-table (list (list 'one 1)
						 (list 'two 2)
						 (list 'three 3)))

		        'done))
          (list "factorial-5.26"
		'(define (factorial-5.26 n)
		    (define (iter product counter)
		        (if (> counter n)
			    product
			    (iter (* product counter)
				  (+ counter 1))))

		    (iter 1 1)))
	  (list "factorial-5.27"
		'(define (factorial-5.27 n)
		    (if (= n 1)
			1
			(* (factorial-5.27 (- n 1)) n))))
	  (list "fib"
		'(define (fib n)
		    (if (< n 2)
			n
			(+ (fib (- n 1))
			   (fib (- n 2))))))))




(define eceval #f)


; evaluator interface #1
(define (make-eceval)
    (let ((machine (make-eceval-machine)))
        (eceval-error-set-ecc (machine-get-register machine 'ecc))

	(set! eceval machine)
	'done))


; evaluator interface #2
(define (eceval-start)
    (if (not eceval)
	(error "ECEVAL START: No available eceval instance"))

    (set-register-contents! eceval 'flag #f)
    (set-register-contents! eceval 'mode 'READ-EVAL-PRINT)
    (start eceval))


; evaluator interface #3
(define (eceval-debug)
    (if (not eceval)
	(error "ECEVAL DEBUG: No available eceval instance"))

    (machine-trace-on eceval)
    (eceval-start))


(define (make-eceval-machine)
    (make-machine (list (list 'simple-apply apply)
		        (list 'true? (lambda (bool) (not (eq? bool #f))))
		        (list 'false? (lambda (bool) (eq? bool #f)))
			(list 'get-false (lambda () #f))
			(list 'adjoin adjoin)
			(list 'list list)
			(list 'error-expression?
			      error-expression?)
			(list 'error-expression-messages
			      error-expression-messages)
                        (list 'self-evaluating? self-evaluating?)
			(list 'variable? variable?)
			(list 'quoted? quoted?)
			(list 'quoted-text quoted-text)
			(list 'assignment? assignment?)
			(list 'assignment-variable assignment-variable)
			(list 'assignment-value-expression
			      assignment-value-expression)
			(list 'definition? definition?)
			(list 'definition-variable definition-variable)
			(list 'definition-value-expression
			      definition-value-expression)
			(list 'lambda? lambda?)
			(list 'lambda-formal-parameters
			      lambda-formal-parameters)
			(list 'lambda-body lambda-body)
			(list 'if? if?)
			(list 'if-predicate if-predicate)
			(list 'if-consequent-expression
			      if-consequent-expression)
			(list 'if-alternative-expression
			      if-alternative-expression)
			(list 'begin? begin?)
			(list 'begin-expressions begin-expressions)
			(list 'sequence-last-expression?
			      sequence-last-expression?)
			(list 'sequence-no-expression?
			      sequence-no-expression?)
			(list 'sequence-first-expression
			      sequence-first-expression)
			(list 'sequence-rest-expressions
			      sequence-rest-expressions)
			(list 'let? let?)
			(list 'let->application
			      let->application)
			(list 'application? application?)
			(list 'application-operator
			      application-operator)
			(list 'application-operands
			      application-operands)
			(list 'application-no-operand?
			      application-no-operand?)
			(list 'application-first-operand
			      application-first-operand)
			(list 'application-rest-operands
			      application-rest-operands)
			(list 'cond? cond?)
			(list 'cond->if cond->if)
			(list 'or? or?)
			(list 'or-predicates
			      or-predicates)
			(list 'or-no-predicate?
			      or-no-predicate?)
			(list 'or-first-predicate
			      or-first-predicate)
			(list 'or-rest-predicates
			      or-rest-predicates)
                        (list 'and? and?)
			(list 'and-predicates
			      and-predicates)
			(list 'and-no-predicate?
			      and-no-predicate?)
			(list 'and-first-predicate
			      and-first-predicate)
			(list 'and-rest-predicates
			      and-rest-predicates)
			(list 'load? load?)
			(list 'load-procedure-name
			      load-procedure-name)
			(list 'load-get-procedure-definition
			      load-get-procedure-definition)
			(list 'primitive-procedure?
			      primitive-procedure?)
			(list 'primitive-procedure-implementation
			      primitive-procedure-implementation)
			(list 'make-compound-procedure
			      (make-compound-procedure-generator))
			(list 'compound-procedure?
			      compound-procedure?)
			(list 'compound-procedure-formal-parameters
			      compound-procedure-formal-parameters)
			(list 'compound-procedure-body
			      compound-procedure-body)
			(list 'compound-procedure-environment
			      compound-procedure-environment)
			(list 'compound-procedure-print
			      compound-procedure-print)
			(list 'make-compiled-procedure
			      (make-compiled-procedure-generator))
			(list 'compiled-procedure?
			      compiled-procedure?)
			(list 'compiled-procedure-entry
			      compiled-procedure-entry)
			(list 'compiled-procedure-environment
			      compiled-procedure-environment)
			(list 'compiled-procedure-print
			      compiled-procedure-print)
			(list 'environment-get-enclosing-environment
			      environment-get-enclosing-environment)
			(list 'environment-extend
			      environment-extend)
			(list 'environment-lookup-binding
			      environment-lookup-binding)
			(list 'environment-modify-binding
			      environment-modify-binding)
			(list 'environment-add-binding
			      environment-add-binding)
			(list 'environment-get-global-environment
			      (make-get-global-environment))
			(list 'lexical-address-lookup
			      lexical-address-lookup)
			(list 'lexical-address-set!
			      lexical-address-set!)
			(list 'eceval-user-print
			      eceval-user-print)
			(list 'eceval-user-printn
			      eceval-user-printn)
			(list 'compile-and-assemble
			      (lambda (exp)
				  (assemble (instruction-sequence-statements
			                        (compile exp
				                        'val
				                        'return
				                        the-empty-ct-environment))
			                    eceval))))
		  eceval-controller-sequence))



(define (make-eceval-error)
    (let ((ecc #f))
        (define (set-ecc ecc-object)
	    (if ecc
		(error "ECEVAL ERROR: ecc had been already setted")
		(set! ecc ecc-object)))

	(define (report error-messages)
	    (register-set-contents! ecc error-messages))

        (lambda (m)
            (cond ((eq? m 'set-ecc) set-ecc)
		  ((eq? m 'report) report)
		  (else (error "ECEVAL ERROR: unexpected message:"
			       m))))))


(define default-eceval-error (make-eceval-error))


(define (eceval-error-set-ecc ecc-object)
    ((default-eceval-error 'set-ecc) ecc-object))


(define (eceval-error . error-messages)
    ((default-eceval-error 'report) error-messages))



(define (eceval-user-print obj)
    (cond ((compound-procedure? obj)
	      (compound-procedure-print obj))
	  ((compiled-procedure? obj)
	      (compiled-procedure-print obj))
	  (else (display obj))))


(define (eceval-user-printn obj)
    (eceval-user-print obj)
    (newline))


(define eceval-controller-sequence '(
(assign env (op environment-get-global-environment))
(assign compapp (label apply-compound-procedure))

(branch (label run-compiled-object-program))


driver-loop
    (perform (op stack-initialize))

    (perform (op printn) (const "EC-EVAL INPUT>"))
    
    (assign exp (op read))
    (assign env (op environment-get-global-environment))

    (test (op eq?) (reg mode) (const READ-EVAL-PRINT))
    (branch (label driver-loop-REP))

    (assign val (op compile-and-assemble) (reg exp))
    (goto (label run-compiled-object-program))

driver-loop-REP
    (assign continue (label driver-loop-print-result))
    (goto (label eval))


driver-loop-print-result
    (perform (op stack-print-statistics))
    (perform (op print) (const "EC-EVAL RESULT> "))
    (perform (op eceval-user-printn) (reg val))

    (goto (label driver-loop))




error-report
    (perform (op print) (const "ERROR>> "))


error-report-loop
    (test (op null?) (reg val))
    (branch (label error-report-done))

    (assign exp (op car) (reg val))
    (perform (op eceval-user-print) (reg exp))
    (perform (op print) (const " "))

    (assign val (op cdr) (reg val))
    (goto (label error-report-loop))


error-report-done
    (perform (op newline))
    (goto (label driver-loop))



error-unexpected-expression-type
    (assign val (const "ERROR: unexpected type of expression:"))
    (assign val (op list) (reg val) (reg exp))
    (goto (label error-report))


error-unexpected-procedure-type
    (assign val (const "ERROR: unexpected type of procedure:"))
    (assign val (op list) (reg val) (reg proc))
    (goto (label error-report))


error-unbound-variable
    (assign val (reg ecc))
    (goto (label error-report))


error-variables-values-do-not-match
    (assign val (reg ecc))
    (goto (label error-report))


error-cond->if
    (assign val (reg ecc))
    (goto (label error-report))


error-apply-primitive
    (assign val (reg ecc))
    (goto (label error-report))




run-compiled-object-program
    (perform (op stack-initialize))

    (assign continue (label driver-loop-print-result))
    (goto (reg val))




eval
    (test (op self-evaluating?) (reg exp))
    (branch (label eval-self-evaluating))

    (test (op variable?) (reg exp))
    (branch (label eval-variable))

    (test (op quoted?) (reg exp))
    (branch (label eval-quoted))

    (test (op if?) (reg exp))
    (branch (label eval-if))

    (test (op cond?) (reg exp))
    (branch (label eval-cond))

    (test (op or?) (reg exp))
    (branch (label eval-or))

    (test (op and?) (reg exp))
    (branch (label eval-and))

    (test (op assignment?) (reg exp))
    (branch (label eval-assignment))

    (test (op definition?) (reg exp))
    (branch (label eval-definition))

    (test (op lambda?) (reg exp))
    (branch (label eval-lambda))

    (test (op begin?) (reg exp))
    (branch (label eval-begin))

    (test (op let?) (reg exp))
    (branch (label eval-let))

    ;     Currently supports loading predefined procedures in the
    ; table "LOAD-PREDEFINED-PROCEDURES" to simplify the testing
    ; process.
    ;     SYNTAX: load <procedure-name>
    (test (op load?) (reg exp))
    (branch (label eval-load))

    (test (op error-expression?) (reg exp))
    (branch (label eval-error-expression))

    (test (op application?) (reg exp))
    (branch (label eval-application))

    (goto (label error-unexpected-expression-type))




eval-error-expression
    (assign val (op error-expression-messages) (reg exp))
    (goto (label error-report))



eval-self-evaluating
    (assign val (reg exp))
    (goto (reg continue))




eval-variable
    (assign ecc (op get-false))

    (assign val (op environment-lookup-binding)
	            (reg env) (reg exp))

    (test (op true?) (reg ecc))
    (branch (label error-unbound-variable))

    (goto (reg continue))




eval-quoted
    (assign val (op quoted-text) (reg exp))
    (goto (reg continue))




eval-lambda
    (assign unev (op lambda-formal-parameters) (reg exp))
    (assign exp (op lambda-body) (reg exp))
    (assign val (op make-compound-procedure)
	            (reg unev) (reg exp) (reg env))
    (goto (reg continue))




eval-cond
    (assign ecc (op get-false))

    (assign exp (op cond->if) (reg exp))

    (test (op true?) (reg ecc))
    (branch (label error-cond->if))

    (goto (label eval-if))




eval-let
    (assign exp (op let->application) (reg exp))
    (goto (label eval-application))




eval-or
    (save continue)
    (assign unev (op or-predicates) (reg exp))


eval-or-loop
    (test (op or-no-predicate?) (reg unev))
    (branch (label eval-or-all-false))

    (save env)
    (save unev)

    (assign exp (op or-first-predicate) (reg unev))
    (assign continue (label eval-or-loop-after-eval))
    (goto (label eval))


eval-or-loop-after-eval
    (restore unev)
    (restore env)

    (test (op true?) (reg val))
    (branch (label eval-or-found-true))

    (assign unev (op or-rest-predicates) (reg unev))
    (goto (label eval-or-loop))


eval-or-all-false
    (assign env (op environment-get-global-environment))
    (assign val (op environment-lookup-binding)
	            (reg env) (const false))

    (restore continue)
    (goto (reg continue))


eval-or-found-true
    (restore continue)
    (goto (reg continue))




eval-and
    (save continue)
    (assign unev (op and-predicates) (reg exp))


eval-and-loop
    (test (op and-no-predicate?) (reg unev))
    (branch (label eval-and-all-true))

    (save env)
    (save unev)

    (assign exp (op and-first-predicate) (reg unev))
    (assign continue (label eval-and-loop-after-eval))
    (goto (label eval))


eval-and-loop-after-eval
    (restore unev)
    (restore env)

    (test (op false?) (reg val))
    (branch (label eval-and-found-false))

    (assign unev (op and-rest-predicates) (reg unev))
    (goto (label eval-and-loop))


eval-and-all-true
    (assign env (op environment-get-global-environment))
    (assign val (op environment-lookup-binding)
	            (reg env) (const true))

    (restore continue)
    (goto (reg continue))


eval-and-found-false
    (restore continue)
    (goto (reg continue))




eval-application
    (save continue)
    (save exp)

    (assign exp (op application-operator) (reg exp))
    (test (op symbol?) (reg exp))
    (branch (label eval-application-symbol-operator))

    (save env)

    (assign continue (label eval-application-after-eval-operator))
    (goto (label eval))


eval-application-symbol-operator
    (assign continue (label eval-application-after-eval-symbol-operator))
    (goto (label eval-variable))
 

eval-application-after-eval-operator
    (restore env)


eval-application-after-eval-symbol-operator
    (restore exp)

    (assign proc (reg val))
    (save proc)

    (assign unev (op application-operands) (reg exp))
    (assign argl (const ()))


eval-application-operands-loop
    (test (op application-no-operand?) (reg unev))
    (branch (label apply))

    (save argl)
    (save env)
    (save unev)

    (assign exp (op application-first-operand) (reg unev))
    (assign continue (label eval-application-operands-loop-after-eval))
    (goto (label eval))


eval-application-operands-loop-after-eval
    (restore unev)
    (restore env)
    (restore argl)

    (assign argl (op adjoin) (reg argl) (reg val))
    (assign unev (op application-rest-operands) (reg unev))

    (goto (label eval-application-operands-loop))




apply
    (restore proc)

    (test (op primitive-procedure?) (reg proc))
    (branch (label apply-primitive-procedure))

    (test (op compound-procedure?) (reg proc))
    (branch (label apply-compound-procedure))

    (test (op compiled-procedure?) (reg proc))
    (branch (label apply-compiled-procedure))

    (goto (label error-unexpected-procedure-type))


apply-primitive-procedure
    (assign proc (op primitive-procedure-implementation)
	             (reg proc))

    (assign ecc (op get-false))

    (assign val (op simple-apply) (reg proc) (reg argl))

    (test (op true?) (reg ecc))
    (branch (label error-apply-primitive))

    (restore continue)
    (goto (reg continue))


apply-compound-procedure
    (assign exp (op compound-procedure-formal-parameters)
	            (reg proc))
    (assign env (op compound-procedure-environment)
	            (reg proc))

    (assign ecc (op get-false))

    (assign env (op environment-extend)
	            (reg env) (reg exp) (reg argl))

    (test (op true?) (reg ecc))
    (branch (label error-variables-values-do-not-match))

    (assign unev (op compound-procedure-body) (reg proc))
    (restore continue)
    (goto (label eval-sequence))


apply-compiled-procedure
    (restore continue)

    (assign val (op compiled-procedure-entry) (reg proc))
    (goto (reg val))




eval-begin
    (assign unev (op begin-expressions) (reg exp))
    (goto (label eval-sequence))




eval-sequence
    (save continue)


eval-sequence-loop ; with tail recursion
    (test (op sequence-last-expression?) (reg unev))
    (branch (label eval-sequence-last-expression))

    (save env)
    (save unev)

    (assign exp (op sequence-first-expression) (reg unev))
    (assign continue (label eval-sequence-loop-after-eval))
    (goto (label eval))


eval-sequence-loop-after-eval
    (restore unev)
    (restore env)

    (assign unev (op sequence-rest-expressions) (reg unev))
    (goto (label eval-sequence-loop))


eval-sequence-last-expression
    (assign exp (op sequence-first-expression) (reg unev))
    (restore continue)
    (goto (label eval))




eval-if
    (save continue)
    (save env)
    (save exp)

    (assign exp (op if-predicate) (reg exp))
    (assign continue (label eval-if-after-eval-predicate))
    (goto (label eval))


eval-if-after-eval-predicate
    (restore exp)
    (restore env)

    (test (op true?) (reg val))
    (branch (label eval-if-apply-consequent-expression))

    (assign exp (op if-alternative-expression) (reg exp))
    (restore continue)
    (goto (label eval))


eval-if-apply-consequent-expression
    (assign exp (op if-consequent-expression) (reg exp))
    (restore continue)
    (goto (label eval))




eval-assignment
    (save continue)
    (save env)
    (save exp)

    (assign exp (op assignment-value-expression) (reg exp))
    (assign continue (label eval-assignment-after-eval-value-expression))
    (goto (label eval))


eval-assignment-after-eval-value-expression
    (restore exp)
    (restore env)

    (assign exp (op assignment-variable) (reg exp))

    (assign ecc (op get-false))

    (perform (op environment-modify-binding)
	         (reg env) (reg exp) (reg val))

    (test (op true?) (reg ecc))
    (branch (label error-unbound-variable))

    (restore continue)
    (goto (reg continue))




eval-definition
    (save continue)
    (save env)
    (save exp)

    (assign exp (op definition-value-expression) (reg exp))
    (assign continue (label eval-definition-after-eval-value-expression))
    (goto (label eval))


eval-definition-after-eval-value-expression
    (restore exp)
    (restore env)

    (assign exp (op definition-variable) (reg exp))
    (perform (op environment-add-binding)
	         (reg env) (reg exp) (reg val))

    (restore continue)
    (goto (reg continue))




eval-load
    (assign exp (op load-procedure-name) (reg exp))
    (assign exp (op load-get-procedure-definition) (reg exp))
    (goto (label eval))))




(define (make-global-environment)
    (let ((eceval-builtin-variables
	     (list (list 'true #t)
		   (list 'false #f)))
	  (eceval-primitive-procedures
	     (list (list '< eceval-<)
		   (list '> eceval->)
		   (list '= eceval-=)
		   (list '<= eceval-<=)
		   (list '>= eceval->=)
		   (list 'not not)
		   (list '+ eceval-+)
		   (list '- eceval--)
		   (list '* eceval-*)
		   (list '/ eceval-/)
		   (list 'remainder eceval-remainder)
	           (list 'car eceval-car)
		   (list 'cdr eceval-cdr)
		   (list 'set-car! eceval-set-car!)
		   (list 'set-cdr! eceval-set-cdr!)
		   (list 'cons cons)
		   (list 'eq? eq?)
		   (list 'equal? equal?)
		   (list 'pair? pair?)
		   (list 'null? null?)
		   (list 'number? number?)
		   (list 'symbol? symbol?)
		   (list 'list list)
		   (list 'read read)
		   (list 'print display)
		   (list 'printn printn)
	           (list 'compile-and-run compiler-run))))
        (let ((var-names (map car
			      eceval-builtin-variables))
	      (var-objects (map cadr
				eceval-builtin-variables))
	      (pproc-names (map car
			       eceval-primitive-procedures))
	      (pproc-objects (map (lambda (binding)
				      (make-primitive-procedure
					  (cadr binding)))
				  eceval-primitive-procedures)))
	    (environment-extend the-empty-environment
				(append var-names pproc-names)
				(append var-objects pproc-objects)))))


(define (make-get-global-environment)
    (let ((env (make-global-environment)))
        (lambda () env)))




(define (eceval-< a b)
    (if (and (number? a) (number? b))
	(< a b)
	(eceval-error "<: arguments need both be number")))


(define (eceval-> a b)
    (if (and (number? a) (number? b))
	(> a b)
	(eceval-error ">: arguments need both be number")))

(define (eceval-= a b)
     (if (and (number? a) (number? b))
	(= a b)
	(eceval-error "=: arguments need both be number")))

(define (eceval-<= a b)
    (if (and (number? a) (number? b))
	(<= a b)
	(eceval-error "<=: arguments need both be number")))

(define (eceval->= a b)
    (if (and (number? a) (number? b))
	(>= a b)
	(eceval-error ">=: arguments need both be number")))

(define (eceval-+ a b)
    (if (and (number? a) (number? b))
	(+ a b)
	(eceval-error "+: arguments need both be number")))

(define (eceval-- a b)
     (if (and (number? a) (number? b))
	(- a b)
	(eceval-error "-: arguments need both be number")))

(define (eceval-* a b)
    (if (and (number? a) (number? b))
	(* a b)
	(eceval-error "*: arguments need both be number")))

(define (eceval-/ a b)
    (if (and (number? a) (number? b))
	(if (= b 0)
	    (eceval-error "/: divided by zero")
	    (/ a b))
	(eceval-error "/: arguments need both be number")))

(define (eceval-remainder a b)
    (if (and (number? a) (number? b))
	(remainder a b)
	(eceval-error "remainder: arguments need both be number")))

(define (eceval-car x)
    (if (pair? x)
	(car x)
	(eceval-error "car: the argument need be a pair")))

(define (eceval-cdr x)
    (if (pair? x)
	(cdr x)
	(eceval-error "cdr: the argument need be a pair")))

(define (eceval-set-car! p val)
    (if (pair? p)
	(set-car! p val)
	(eceval-error "set-car!: the first argument need be a pair")))

(define (eceval-set-cdr! p val)
    (if (pair? p)
	(set-cdr! p val)
	(eceval-error "set-cdr!: the first argument need be a pair")))




; --- SECTION 3: The Register-Machine Simulator ---
; simulator interface #1
(define (make-machine op-table controller-text)
    (let ((machine (make-basic-machine)))
        (machine-install-operations machine op-table)
	(machine-install-instruction-sequence machine
	                                      (assemble controller-text machine))
	machine))




(define (make-register reg-name)
    (let ((register-contents '*UNASSIGNED*)
	  (register-trace-enabled #f))
        (define (dispatch m)
            (cond ((eq? m 'get-contents)
		      (if (eq? register-contents '*UNASSIGNED*)
			  (error "REGISTER: unassigned register:"
                                 reg-name)
			  register-contents))
		  ((eq? m 'set-contents!)
		      (lambda (new-contents)
			  (if register-trace-enabled
			      (begin (printn "[DEBUG INFO]" "REGISTER"
					     "name:" reg-name
					     "old:" register-contents
				             "new:" new-contents)))
			  (set! register-contents new-contents)))
		  ((eq? m 'trace)
		      (lambda (m)
			  (cond ((eq? m 'on)
                                    (begin (set! register-trace-enabled #t)
				           (printn "REGISTER: trace on")))
				((eq? m 'off)
                                    (begin (set! register-trace-enabled #f)
				           (printn "REGISTER: trace off")))
				(else (error "REGISTER TRACE: unexpected message:" m)))))
		  (else (error "REGISTER: unexpected message:" m))))

	dispatch))


(define (register-get-contents reg)
    (reg 'get-contents))


(define (register-set-contents! reg new-contents)
    ((reg 'set-contents!) new-contents))


(define (register-trace-on reg)
    ((reg 'trace) 'on))


(define (register-trace-off reg)
    ((reg 'trace) 'off))




(define (make-stack)
    (let ((stack '())
	  (save-number 0)
	  (current-depth 0)
	  (max-depth 0))
        (define (push value)
	    (set! stack (cons value stack))
	    (set! save-number (+ save-number 1))
	    (set! current-depth (+ current-depth 1))
	    (set! max-depth (max current-depth max-depth)))

	(define (pop)
	    (if (null? stack)
		(error "STACK: empty stack")
		(let ((value (car stack)))
		    (set! stack (cdr stack))
		    (set! current-depth (- current-depth 1))
		    value)))

	(define (initialize)
	    (set! stack '())
	    (set! save-number 0)
	    (set! current-depth 0)
	    (set! max-depth 0)
	    'done)

	(define (print-statistics)
	   (printn "[DEBUG INFO]" "STACK"
                   "max-depth:" max-depth
                   "save-number:" save-number))

	(define (dispatch m)
	    (cond ((eq? m 'push) push)
		  ((eq? m 'pop) (pop))
		  ((eq? m 'initialize) (initialize))
		  ((eq? m 'print-statistics) (print-statistics))
		  (else (error "STACK: unexpected message:" m))))


	dispatch))


(define (stack-push stack value)
    ((stack 'push) value))


(define (stack-pop stack)
    (stack 'pop))


(define (stack-initialize stack)
    (stack 'initialize))


(define (stack-print-statistics stack)
   (stack 'print-statistics))




(define (make-basic-machine)
    (let ((machine-stack (make-stack))
	  (machine-instruction-sequence '())
	  (machine-labels '())
	  (machine-instruction-count 0)
	  (machine-trace-enabled #f)
	  (machine-suspended-next-procedure #f)
	  (pc (make-register 'pc))
	  (flag (make-register 'flag))
	  (ecc (make-register 'ecc)))
        (let ((machine-operation-table
		 (list (list '< <)
		       (list '<= <=)
		       (list '> >)
		       (list '>= >=)
		       (list '= =)
		       (list '+ +)
		       (list '- -)
		       (list '* *)
		       (list '/ /)
		       (list 'rem remainder)
		       ;     simplify explicit-control evaluater by directly
		       ; using list operatiions provided by underlying scheme
		       ; since calling subroutines implemented in the book is
		       ; more complicated than simply using operation subex-
		       ; pression from register-machine language and which
		       ; choice we made is irrelevent to these exercises.
		       (list 'car car)
		       (list 'cdr cdr)
		       (list 'set-car! set-car!)
		       (list 'set-cdr! set-cdr!)
		       (list 'cons cons)
		       (list 'eq? eq?)
		       (list 'pair? pair?)
		       (list 'null? null?)
		       (list 'number? number?)
		       (list 'symbol? symbol?)
		       (list 'read read)
		       (list 'print display)
		       (list 'printn printn)
		       (list 'newline newline)
		       (list 'stack-initialize
		             (lambda ()
			         (stack-initialize machine-stack)))
		       (list 'stack-print-statistics
			     (lambda ()
			         (stack-print-statistics machine-stack)))))
	       (machine-register-table
		   (list (list 'pc pc)
		         (list 'flag flag)
			 (list 'ecc ecc))))


	    (define (allocate-register reg-name)
	        (let ((record (assoc reg-name machine-register-table)))
		    (if record
			(cadr record)
			(if (symbol? reg-name)
			    (let ((new-reg (make-register reg-name)))
			        (set! machine-register-table
			              (cons (list reg-name new-reg)
					    machine-register-table))
			        new-reg)
			    (error "MACHINE: illegal register name:"
				   reg-name)))))


	    (define (get-register reg-name)
	        (let ((record (assoc reg-name machine-register-table)))
		    (if record
			(cadr record)
			(error "MACHINE: No such register named:"
			       reg-name))))


	    (define (execute)
	        (let ((insts (register-get-contents pc)))
		    (if (null? insts)
			'success
			(let ((next-inst (car insts)))
			    (let ((next-proc
				     (instruction-execution-procedure next-inst)))
			        (set! machine-instruction-count
			              (+ machine-instruction-count 1))
			        (if machine-trace-enabled
			            (begin (let ((label-name (instruction-label-name next-inst)))
					       (if label-name
					           (printn label-name)))
				           (printn "\t" (instruction-text next-inst))))
				(let ((breakpoint-name (instruction-breakpoint-name next-inst)))
				    (if breakpoint-name
					(begin (printn "BREAKPOINT NAME:" breakpoint-name)
					       (set! machine-suspended-next-procedure
						     next-proc))
			                (begin (next-proc)
			                       (execute)))))))))


	    (define (dispatch m)
	        (cond ((eq? m 'get-stack) machine-stack)
		      ((eq? m 'get-operations) machine-operation-table)
		      ((eq? m 'get-labels) machine-labels)
		      ((eq? m 'get-instruction-sequence) machine-instruction-sequence)
		      ((eq? m 'get-suspended-next-procedure)
		          machine-suspended-next-procedure)
		      ((eq? m 'reset-suspended-next-procedure!)
		          (set! machine-suspended-next-procedure #f))
		      ((eq? m 'allocate-register) allocate-register)
		      ((eq? m 'get-register) get-register)
		      ((eq? m 'start)
		          (begin (set! machine-suspended-next-procedure #f)
		                 (register-set-contents! pc machine-instruction-sequence)
			         (execute)))
		      ((eq? m 'install-labels)
		          (lambda (labels)
			      (set! machine-labels
				    labels)))
		      ((eq? m 'install-operations)
		          (lambda (ops)
			      (set! machine-operation-table
				    (append machine-operation-table
					    ops))))
		      ((eq? m 'install-instruction-sequence)
		          (lambda (seq)
			      (set! machine-instruction-sequence
				    seq)))
		      ((eq? m 'get-instruction-count)
		          (lambda (m)
			      (cond ((eq? m '=) machine-instruction-count)
				    ((eq? m 'reset)
				        (begin (set! machine-instruction-count 0)
				               (printn 'reseted)))
				    (else (error "GET INSTRUCTION COUNT: unexpected message:"
                                                 m)))))
		      ((eq? m 'trace)
		          (lambda (m)
			      (cond ((eq? m 'on)
				        (begin (set! machine-trace-enabled #t)
					       (printn "machine: trace on")))
				    ((eq? m 'off)
					(begin (set! machine-trace-enabled #f)
					       (printn "machine: trace off")))
				    (else (error "MACHINE TRACE: unexpected message")))))
		      ((eq? m 'execute)
		          (execute))
		      (else (error "MACHINE: unexpected message:" m))))


	    dispatch)))


(define (machine-get-stack machine)
    (machine 'get-stack))


(define (machine-get-operations machine)
    (machine 'get-operations))


(define (machine-get-labels machine)
    (machine 'get-labels))


(define (machine-get-instruction-sequence machine)
    (machine 'get-instruction-sequence))


(define (machine-get-suspended-next-procedure machine)
    (machine 'get-suspended-next-procedure))


(define (machine-reset-suspended-next-procedure! machine)
    (machine 'reset-suspended-next-procedure!))


(define (machine-allocate-register machine reg-name)
    ((machine 'allocate-register) reg-name))


(define (machine-get-register machine reg-name)
    ((machine 'get-register) reg-name))


(define (machine-install-labels machine labels)
    ((machine 'install-labels) labels))


(define (machine-install-operations machine ops)
    ((machine 'install-operations) ops))


(define (machine-install-instruction-sequence machine seq)
    ((machine 'install-instruction-sequence) seq))


(define (machine-execute machine)
    (machine 'execute))



; simulator interface #2
(define (start machine) (machine 'start))


; simulator interface #3
(define (get-register-contents machine reg-name)
    (register-get-contents (machine-get-register machine reg-name)))


; simulator interface #4
(define (set-register-contents! machine reg-name new-contents)
    ;     Most registers are not allocated before the first time we "start"
    ; the machine. For this reason, first, we have to allocate the requested
    ; register, then we can set it to the new contents as we need.
    (let ((reg (machine-allocate-register machine reg-name)))
        (register-set-contents! reg
			        new-contents)))


; simulator interface #5
(define (machine-get-instruction-count machine)
    ((machine 'get-instruction-count) '=))


; simulator interface #6
(define (machine-reset-instruction-count! machine)
    ((machine 'get-instruction-count) 'reset))


; simulator interface #7
(define (machine-trace-on machine)
    ((machine 'trace) 'on))


; simulator interface #8
(define (machine-trace-off machine)
    ((machine 'trace) 'off))


; simulator interface #9
(define (machine-trace-register-on machine reg-name)
    ;     Most registers are not allocated before the first time we "start"
    ; the machine. For this reason, first, we have to allocate the requested
    ; register, then we can turn its trace option "on" as we need.
    (let ((reg (machine-allocate-register machine reg-name)))
        ((reg 'trace) 'on)))


; simulator interface #10
(define (machine-trace-register-off machine reg-name)
    (let ((reg (machine-get-register machine reg-name)))
        ((reg 'trace) 'off)))


(define (find-instruction insts line-num)
    (if (= line-num 1)
	(car insts)
	(find-instruction (cdr insts) (- line-num 1))))

; simulator interface #11
(define (set-breakpoint machine breakpoint-name line-num)
    (let ((insts (machine-get-instruction-sequence machine)))
        (instruction-set-breakpoint-name! (find-instruction insts line-num)
					  breakpoint-name)
	'done))


; simulator interface #12
(define (proceed-machine machine)
    (let ((next-proc (machine-get-suspended-next-procedure machine)))
        (if next-proc
            (begin (next-proc)
	           (machine-reset-suspended-next-procedure! machine)
	           (machine-execute machine))
	    (error "MACHINE: machine has not met breakpoints yet"))))


; simulator interface #13
(define (cancel-breakpoint machine breakpoint-name line-num)
    (let ((insts (machine-get-instruction-sequence machine)))
        (instruction-set-breakpoint-name! (find-instruction insts line-num)
					  #f)
	'done))


; simulator interface #14
(define (cancel-all-breakpoints machine)
    (let ((insts (machine-get-instruction-sequence machine)))
        (for-each (lambda (inst)
		      (instruction-set-breakpoint-name! inst
							#f))
		  insts)
	'done))
 



(define (assemble controller-text machine)
    (separate-controller-text (adjoin controller-text ;     Fix "label can't be the
				      '(nop))         ; last instruction" bug intro-
			                              ; duced from implementing the
					              ; machine trace functionality.
			      (lambda (insts labels)
				  (machine-install-labels machine labels)
		                  (transform-instruction-sequence machine insts labels)
				  insts)))



(define (separate-controller-text text continuation)
    (if (null? text)
	(continuation '() '())
	(separate-controller-text
	    (cdr text)
	    (lambda (insts labels)
		(let ((inst (car text)))
		    (if (symbol? inst)
			(continuation insts
				      (cons (make-label inst insts)
                                            labels))
			(continuation (cons (make-instruction inst)
					    insts)
				      labels)))))))



(define (transform-instruction-sequence machine insts labels)
    (for-each (lambda (inst)
		  (instruction-set-execution-procedure!
		      inst
		      (generate-instruction-execution-procedure
			  machine (instruction-text inst) labels)))
	      insts))



; (list "text" "instruction-execution-procedure" "label-name" "breakpoint-name")
(define (make-instruction text)
    (list text #f #f #f))


(define (instruction-text inst)
    (car inst))


(define (instruction-execution-procedure inst)
    (cadr inst))


(define (instruction-set-execution-procedure! inst proc)
    (set-car! (cdr inst) proc))


(define (instruction-label-name inst)
    (caddr inst))


(define (instruction-set-label-name! inst label-name)
    (set-car! (cddr inst) label-name))


(define (instruction-breakpoint-name inst)
    (cadddr inst))


(define (instruction-set-breakpoint-name! inst breakpoint-name)
    (set-car! (cdddr inst) breakpoint-name))



(define (make-label label-name insts)
    (instruction-set-label-name! (car insts) label-name)
    (cons label-name insts))


(define (label-lookup labels label-name)
    (let ((record (assoc label-name labels)))
        (if record
	    (cdr record) ; insts
            (error "LABEL: no such label named:"
		   label-name))))




(define (generate-instruction-execution-procedure machine inst-text labels)
    (cond ((instruction-text-type-assign? inst-text)
	      (analyze-assign machine inst-text labels))
	  ((instruction-text-type-test? inst-text)
	      (analyze-test machine inst-text))
	  ((instruction-text-type-branch? inst-text)
	      (analyze-branch machine inst-text labels))
	  ((instruction-text-type-goto? inst-text)
	      (analyze-goto machine inst-text labels))
	  ((instruction-text-type-perform? inst-text)
	      (analyze-perform machine inst-text))
	  ((instruction-text-type-save? inst-text)
	      (analyze-save machine inst-text))
	  ((instruction-text-type-restore? inst-text)
	      (analyze-restore machine inst-text))
	  ((instruction-text-type-nop? inst-text)
	      (analyze-nop machine inst-text))
	  (else (error "ASSEMBLE: unexpected instruction type:"
		       inst-text))))


(define (instruction-text-type? inst type)
    (and (pair? inst)
	 (let ((first-item (car inst)))
	     (and (symbol? first-item)
	          (eq? first-item type)))))


(define (instruction-text-type-assign? inst)
    (instruction-text-type? inst 'assign))


(define (instruction-text-type-test? inst)
    (instruction-text-type? inst 'test))


(define (instruction-text-type-branch? inst)
    (instruction-text-type? inst 'branch))


(define (instruction-text-type-goto? inst)
    (instruction-text-type? inst 'goto))


(define (instruction-text-type-perform? inst)
    (instruction-text-type? inst 'perform))


(define (instruction-text-type-save? inst)
    (instruction-text-type? inst 'save))


(define (instruction-text-type-restore? inst)
    (instruction-text-type? inst 'restore))


(define (instruction-text-type-nop? inst)
    (instruction-text-type? inst 'nop))




; try to allocate "accept register", since there is no pre-allocation of machine registers
(define (analyze-assign machine inst-text labels)
    (let ((memo-reg #f)
	  (reg-name (assign-accept-register-name inst-text))
	  (value-exp (assign-value-expression inst-text))
	  (pc (machine-get-register machine 'pc)))
        (let ((value-proc
		 (cond ((expression-operation? value-exp)
		           (expression-operation-analyze machine value-exp))
		       ((pair? value-exp)
		           (let ((exp (car value-exp)))
		               (cond ((expression-register? exp)
			                 (expression-register-analyze machine exp))
			             ((expression-constant? exp)
				         (expression-constant-analyze exp))
			             ((expression-label? exp)
				         (expression-label-analyze exp labels))
				     (else (error "ASSEMBLE: illegal instruction error:"
						  inst-text)))))
		       (else (error "ASSEMBLE: illegal instruction format:"
				    inst-text)))))
	    (lambda ()
	        (if (not memo-reg)
	            (set! memo-reg (machine-allocate-register machine reg-name)))
	        (register-set-contents! memo-reg (value-proc))
                (advance-pc! pc)))))


(define (assign-accept-register-name inst-text)
    (cadr inst-text))


(define (assign-value-expression inst-text)
    (cddr inst-text))


(define (advance-pc! pc)
    (register-set-contents! pc
			    (cdr (register-get-contents pc))))




(define (analyze-test machine inst-text)
    (let ((value-exp (test-value-expression inst-text))
	  (flag (machine-get-register machine 'flag))
	  (pc (machine-get-register machine 'pc)))
        (let ((value-proc 
		 (if (expression-operation? value-exp)
		     (expression-operation-analyze machine value-exp)
		     (error "ASSEMBLE: illegal instruction format:"
			    inst-text))))
	    (lambda ()
	        (register-set-contents! flag (value-proc))
		(advance-pc! pc)))))


(define (test-value-expression inst-text)
    (cdr inst-text))




(define (analyze-branch machine inst-text labels)
    (let ((dest-exp (branch-destination-expression inst-text))
	  (flag (machine-get-register machine 'flag))
	  (pc (machine-get-register machine 'pc)))
        (let ((dest-proc
		 (if (expression-label? dest-exp)
	             (expression-label-analyze dest-exp labels)
	             (error "ASSEMBLE: illegal expression format:"
			    inst-text))))
            (lambda ()
	        (if (register-get-contents flag)
		    (register-set-contents! pc (dest-proc))
		    (advance-pc! pc))))))


(define (branch-destination-expression inst-text)
    (cadr inst-text))




(define (analyze-goto machine inst-text labels)
    (let ((dest-exp (goto-destination-expression inst-text))
	  (pc (machine-get-register machine 'pc)))
        (let ((dest-proc
		 (cond ((expression-register? dest-exp)
			   (expression-register-analyze machine dest-exp))
		       ((expression-label? dest-exp)
			   (expression-label-analyze dest-exp labels))
		       (else (error "ASSEMBLE: illegal expression format:"
			            inst-text)))))
	    (lambda ()
	        (register-set-contents! pc (dest-proc))))))


(define (goto-destination-expression inst-text)
    (cadr inst-text))




(define (analyze-perform machine inst-text)
    (let ((action-exp (perform-action-expression inst-text))
	  (pc (machine-get-register machine 'pc)))
        (let ((action-proc
	         (if (expression-operation? action-exp)
		     (expression-operation-analyze machine action-exp)
		     (error "ASSEMBLE: illegal instruction format:"
			    inst-text))))
	    (lambda ()
	        (action-proc)
		(advance-pc! pc)))))


(define (perform-action-expression inst-text)
    (cdr inst-text))




(define (analyze-save machine inst-text)
    (let ((memo-reg #f)
	  (reg-name (save-register-name inst-text))
	  (stack (machine-get-stack machine))
	  (pc (machine-get-register machine 'pc)))
        (lambda ()
	    (if (not memo-reg)
	        (set! memo-reg (machine-get-register machine reg-name)))
	    (stack-push stack (register-get-contents memo-reg))
            (advance-pc! pc))))


(define (save-register-name inst-text)
    (cadr inst-text))



; try to allocate register, since there is no pre-allocation of machine registers
(define (analyze-restore machine inst-text)
    (let ((memo-reg #f)
	  (reg-name (restore-register-name inst-text))
	  (stack (machine-get-stack machine))
	  (pc (machine-get-register machine 'pc)))
        (lambda ()
	    (if (not memo-reg)
	        (set! memo-reg (machine-allocate-register machine reg-name)))
	    (register-set-contents! memo-reg
				    (stack-pop stack))
	    (advance-pc! pc))))


(define (restore-register-name inst-text)
    (cadr inst-text))



(define (analyze-nop machine inst-text)
    (let ((pc (machine-get-register machine 'pc)))
        (lambda ()
	    (advance-pc! pc))))




(define (expression-primitive? exp type)
    (and (pair? exp)
	 (eq? (car exp) type)
	 (= (length exp) 2)))


(define (expression-register? exp)
    (expression-primitive? exp 'reg))


(define (expression-constant? exp)
    (expression-primitive? exp 'const))


(define (expression-label? exp)
    (expression-primitive? exp 'label))



(define (expression-primitive-operand exp)
    (cadr exp))


(define (expression-register-name exp)
    (expression-primitive-operand exp))


(define (expression-constant-value exp)
    (expression-primitive-operand exp))


(define (expression-label-name exp)
    (expression-primitive-operand exp))



(define (expression-register-analyze machine exp)
    (let ((memo-reg #f)
	  (reg-name (expression-register-name exp)))
        (lambda ()
	    (if (not memo-reg)
	        (set! memo-reg (machine-get-register machine reg-name)))
            (register-get-contents memo-reg))))


(define (expression-constant-analyze exp)
    (let ((value (expression-constant-value exp)))
        (lambda () value)))


(define (expression-label-analyze exp labels)
    (let ((insts (label-lookup labels
			       (expression-label-name exp))))
        (lambda () insts)))




(define (expression-operation? exp)
    (and (pair? exp)
	 (let ((op-name-exp (car exp)))
	     (and (pair? op-name-exp)
		  (eq? (car op-name-exp) 'op)
		  (= (length op-name-exp) 2)))))


(define (expression-operation-name exp)
    (cadr (car exp)))


(define (expression-operation-arguments exp)
    (cdr exp))


(define (expression-operation-analyze machine exp)
    (let ((proc (primitive-operation-lookup (machine-get-operations machine)
					    (expression-operation-name exp)))
	  (arg-procs (map (lambda (arg)
			      (cond ((expression-register? arg)
				        (expression-register-analyze machine arg))
				    ((expression-constant? arg)
				        (expression-constant-analyze arg))
				    (else (error "ASSEMBLE: illegal operation argument format:"
						 arg))))
			  (expression-operation-arguments exp))))
        (lambda ()
	    (apply proc (map (lambda (arg-proc) (arg-proc))
			     arg-procs)))))


(define (primitive-operation-lookup ops name)
    (let ((record (assoc name ops)))
        (if record
	    (cadr record)
	    (error "ASSEMBLE: No such primitive operation named:"
		   name))))




; --- SECTION 4: The Compiler ---
(define fact-rec
'(define (factorial n)
    (if (= n 1)
	1
	(* (factorial (- n 1)) n))))


(define fact-rec-alt
'(define (factorial-alt n)
    (if (= n 1)
	1
	(* n (factorial-alt (- n 1))))))


(define fact-iter
'(define (factorial n)
    (define (iter product counter)
        (if (> counter n)
	    product
	    (iter (* counter product)
		  (+ counter 1))))

    (iter 1 1)))



(define ALL-COMPILER-REGISTERS '(proc argl env continue val))

(define SUPPORTED-OPEN-CODING-OPERATIONS '(= * - +))
(define GENERALIZED-OPEN-CODING-OPERATIONS '(+ *))




(define (make-compiler-label-generator)
    (let ((label-number 0))
        (lambda (prefix)
	    (let ((n label-number))
	        (set! label-number
		      (+ label-number 1))
		(string->symbol
		    (string-append (symbol->string prefix)
				   (number->string n)))))))


(define make-compiler-label (make-compiler-label-generator))




; compiler interface #1
(define (compile exp target linkage ct-env)
    (cond ((self-evaluating? exp)
	      (compile-self-evaluating exp target linkage ct-env))
	  ((variable? exp)
	      (compile-variable exp target linkage ct-env))
	  ((quoted? exp)
	      (compile-quoted exp target linkage ct-env))
	  ((assignment? exp)
	      (compile-assignment exp target linkage ct-env))
	  ((definition? exp)
	      (compile-definition exp target linkage ct-env))
	  ((if? exp)
	      (compile-if exp target linkage ct-env))
	  ((cond? exp)
	      (compile-if (cond->if exp) target linkage ct-env))
	  ((begin? exp)
	      (compile-sequence (begin-expressions exp)
				target
				linkage
				ct-env))
	  ((lambda? exp)
	      (compile-lambda exp target linkage ct-env))
	  ((and (application? exp)
		(let ((op (application-operator exp)))
		    (and (member op
                                 SUPPORTED-OPEN-CODING-OPERATIONS)
			 (not (find-variable ct-env op)))))
	      (if (member (application-operator exp)
			  GENERALIZED-OPEN-CODING-OPERATIONS)
		  (compile-generalized-open-coding exp
						   target
						   linkage
						   ct-env)
		  (if (= (application-operand-number
			     (application-operands exp)) 2)
	              (compile-open-coding exp target linkage ct-env)
		      (error "COMPILE: unexpected number of arguments"
			     exp))))
	  ((application? exp)
	      (compile-application exp target linkage ct-env))
	  (else (error "COMPILE: unexpected type of expression"
		       exp))))



; compiler interface #2
(define (compiler-run exp)
    (if (not eceval)
	(error "COMPILER RUN: No available eceval instance"))

    (let ((insts (assemble (instruction-sequence-statements
			       (compile exp
				        'val
				        'return
				        the-empty-ct-environment))
			   eceval)))
        (set-register-contents! eceval 'val insts)
	(set-register-contents! eceval 'flag #t)

	(start eceval)))


; compiler interface #3
(define (compiler-print exp target linkage)
    (print-statements
        (compile exp target linkage the-empty-ct-environment)))


; compiler interface #4
(define (compiler-start)
    (if (not eceval)
	(error "COMPILER START: No available eceval instance"))


	(set-register-contents! eceval 'flag #f)
	(set-register-contents! eceval 'mode 'READ-COMPILE-EXECUTE-PRINT)

	(start eceval))




(define (spread-arguments args op-iseq ct-env)
    (instruction-sequence-preserve
        '(env continue)
        (compile (application-first-operand args)
		 'arg1
		 'next
		 ct-env)
	(instruction-sequence-preserve
	    '(arg1 env continue)
	    (compile (application-first-operand
	                 (application-rest-operands args))
		     'arg2
		     'next
		     ct-env)
	    op-iseq)))


(define (compile-open-coding exp target linkage ct-env)
    (attach-linkage
        linkage
	(spread-arguments
	    (application-operands exp)
            (make-instruction-sequence
                '(arg1 arg2)
	        '(,target)
	        `((assign ,target (op ,(application-operator exp))
		                      (reg arg1) (reg arg2))))
	    ct-env)))


(define (spread-first-argument args op-iseq ct-env)
    (instruction-sequence-preserve
	'(arg1 env continue)
	(compile (application-first-operand args)
		 'arg2
		 'next
		 ct-env)
	op-iseq))


(define (compile-generalized-open-coding exp target linkage ct-env)
    (define (iter operator operands)
        (if (application-no-operand? operands)
	    (make-instruction-sequence
	        '(arg1)
		`(,target)
		`((assign ,target (reg arg1))))
	    (instruction-sequence-append
                (spread-first-argument
	            operands
                    (make-instruction-sequence
                        '(arg1 arg2)
	                '(arg1)
	                `((assign arg1 (op ,operator)
		                           (reg arg1) (reg arg2))))
		    ct-env)
		(iter operator
		      (application-rest-operands operands)))))



    (let ((operator (application-operator exp))
	  (operands (application-operands exp)))
        (let ((n (application-operand-number operands)))
            (cond ((= n 1)
		      (error "COMPILE: unexpected number of operands"
			     exp))
	          ((= n 2)
     	              (compile-open-coding exp target linkage ct-env))
                  (else
	              (attach-linkage linkage
                          (instruction-sequence-append
                              (compile-open-coding exp 'arg1 'next ct-env)
	                      (iter operator 
		                    (application-rest-operands
			                (application-rest-operands operands))))))))))




(define (compile-linkage linkage)
    (cond ((eq? linkage 'return)
	      (make-instruction-sequence
		  '(continue)
		  '()
		  '((goto (reg continue)))))
	  ((eq? linkage 'next)
	      the-empty-instruction-sequence)
	  (else (make-instruction-sequence
		    '()
		    '()
		    `((goto (label ,linkage)))))))



(define (attach-linkage linkage iseq)
    (instruction-sequence-preserve '(continue)
				   iseq
				   (compile-linkage linkage)))



(define (compile-self-evaluating exp target linkage ct-env)
    (attach-linkage linkage
		    (make-instruction-sequence
		        '()
			`(,target)
			`((assign ,target (const ,exp))))))



(define (compile-quoted exp target linkage ct-env)
    (attach-linkage linkage
		    (make-instruction-sequence
		        '()
			`(,target)
			`((assign ,target (const ,(quoted-text exp)))))))



(define (compile-variable exp target linkage ct-env)
    (let ((lexical-address (find-variable ct-env exp)))
        (attach-linkage linkage
            (if lexical-address
		(make-instruction-sequence
		    '(env)
	            `(,target)
		    `((assign ,target (op lexical-address-lookup)
				          (reg env) (const ,lexical-address))))
		(make-instruction-sequence
		    '()
	            `(env ,target)
	            `((assign env (op environment-get-global-environment))
		      (assign ,target (op environment-lookup-binding)
				          (reg env) (const ,exp))))))))



(define (compile-assignment exp target linkage ct-env)
    (let ((lexical-address (find-variable ct-env
					  (assignment-variable exp))))
        (attach-linkage linkage
            (if lexical-address
	        (instruction-sequence-preserve
		    '(env)
	            (compile (assignment-value-expression exp)
		             'val
		             'next
			     ct-env)
		    (make-instruction-sequence
		        '(env)
			`(,target)
			`((perform (op lexical-address-set!)
				       (reg env)
				       (const ,lexical-address)
				       (reg val)))))
		(instruction-sequence-append
	            (compile (assignment-value-expression exp)
		             'val
		             'next
			     ct-env)
		    (make-instruction-sequence
		        '()
			`(env ,target)
			`((assign env (op environment-get-global-environment))
			  (perform (op environment-modify-binding)
				       (reg env)
				       (const ,(assignment-variable exp))
				       (reg val)))))))))



(define (compile-definition exp target linkage ct-env)
    (attach-linkage linkage
		    (instruction-sequence-preserve
		        '(env)
			(compile (definition-value-expression exp)
				 'val
				 'next
				 ct-env)
			(make-instruction-sequence
			    '(env)
			    `(,target)
			    `((perform (op environment-add-binding)
				           (reg env)
					   (const ,(definition-variable exp))
					   (reg val))
			      (assign ,target (reg val)))))))



(define (compile-if exp target linkage ct-env)
    (let ((clabel (make-compiler-label 'if-consequent))
	  (alabel (make-compiler-label 'if-alternative))
	  (dlabel (make-compiler-label 'if-done)))
        (let ((clinkage
		 (if (eq? linkage 'next)
                     dlabel
		     linkage)))
	    (let ((piseq (compile (if-predicate exp)
			          'val
			          'next
				  ct-env))
	          (ciseq (compile (if-consequent-expression exp)
			          target
			          clinkage
				  ct-env))
      	          (aiseq (compile (if-alternative-expression exp)
			          target
			          linkage
				  ct-env)))
	        (instruction-sequence-preserve
	            '(env continue)
		    piseq
		    (instruction-sequence-append
		        (make-instruction-sequence
		            '(val)
			    '()
			    `((test (op false?) (reg val))
			      (branch (label ,alabel))))
		        (instruction-sequence-parallel
		            (instruction-sequence-append clabel ciseq)
			    (instruction-sequence-append alabel aiseq))
		        dlabel))))))



(define (compile-sequence exp target linkage ct-env)
    (if (sequence-last-expression? exp)
	(compile (sequence-first-expression exp) target linkage ct-env)
	(instruction-sequence-preserve
	    '(env continue)
	    (compile (sequence-first-expression exp)
                     target
		     'next
		     ct-env)
	    (compile-sequence (sequence-rest-expressions exp)
			      target
			      linkage
			      ct-env))))



(define (compile-lambda exp target linkage ct-env)
    (let ((proc-entry-label (make-compiler-label 'procedure-entry-point))
	  (lambda-done-label (make-compiler-label 'lambda-done)))
        (let ((first-part-linkage (if (eq? linkage 'next)
				      lambda-done-label
				      linkage)))
	    (instruction-sequence-append
	        (instruction-sequence-tack
		    (attach-linkage first-part-linkage
		                    (make-instruction-sequence
		                        '(env)
			                `(,target)
					;     the book forgets that "operation subexpression"
					; do not support "label" type operand, and here we
					; add an instruction to temporarily store this label
					; in a register and then use this register as operand,
					; avoids explicitly using a label as operand.
			                `((assign ,target (label ,proc-entry-label))
					  (assign ,target
				            (op make-compiled-procedure)
				                (reg ,target)
					        (reg env)))))
		    (compile-procedure-body exp proc-entry-label ct-env))
		lambda-done-label))))



(define (compile-procedure-body exp proc-entry-label ct-env)
    (instruction-sequence-append
        (make-instruction-sequence
	    '(env proc argl)
	    '(env)
	    `(,proc-entry-label
	      (assign env (op compiled-procedure-environment)
		              (reg proc))
	      (assign env (op environment-extend)
		              (reg env)
			      (const ,(lambda-formal-parameters exp))
			      (reg argl))))
	(compile-sequence (transform-lambda-body (lambda-body exp))
		          'val
		          'return
			  (ct-environment-extend ct-env
						 (lambda-formal-parameters exp)))))



(define (scan-out-definition seq)
    (define (iter seq defs usages)
        (if (sequence-no-expression? seq)
	    (cons (reverse defs)
		  (reverse usages))
	    (let ((exp (sequence-first-expression seq)))
	        (if (definition? exp)
		    (iter (sequence-rest-expressions seq)
			  (cons exp defs)
			  usages)
		    (iter (sequence-rest-expressions seq)
			  defs
			  (cons exp usages))))))

    (iter seq '() '()))



(define (transform-lambda-body body)
    (let ((result (scan-out-definition body)))
        (let ((defs (car result))
	      (usages (cdr result)))
	    (if (null? defs)
		body
	        (let ((def-vars (map (lambda (def)
				         (definition-variable def))
				     defs))
		      (def-vals (map (lambda (def)
				         (definition-value-expression def))
				     defs))
		      (assignments (map (lambda (def)
				            (make-assignment
					        (definition-variable def)
					        (definition-value-expression def)))
				        defs)))
	            (let ((lambda-exp (make-lambda def-vars
					           (sequence-append
						       assignments
						       usages))))
                        (make-sequence
		            (make-application lambda-exp
				              (map (lambda (def)
						   ;     Our register-machine simulator
						   ; takes registers with the special sym-
						   ; bol "*unassigned*" as their value as
						   ; unassigned, and it report error about
						   ; any usages of unassigned registers.
						   ; Moreover, our compiler will make the
						   ; object program put this special symbol
						   ; in the register "val" when evaluating
						   ; the assignment value expression if here
						   ; we choose "*unassigned*" as in the book.
						       ''*scheme-unassigned*)
					           def-vars)))))))))



(define (compile-application exp target linkage ct-env)
    (instruction-sequence-preserve
        '(env continue)
	(compile (application-operator exp) 'proc 'next ct-env)
	(instruction-sequence-preserve
	    '(proc continue)
            (compile-application-argument-list (application-operands exp) ct-env)
	    (compile-application-apply target linkage))))


(define (compile-application-argument-list operands ct-env)
    (let ((args (reverse operands)))
        (if (application-no-operand? args)
	    (make-instruction-sequence
	        '()
		'(argl)
		'((assign argl (const ()))))
	    (instruction-sequence-preserve
	        '(env)
		(instruction-sequence-append
		    (compile (application-first-operand args) 'val 'next ct-env)
		    (make-instruction-sequence
		        '(val)
			'(argl)
			'((assign argl (op list) (reg val)))))
		(compile-application-argument-list-continue
		    (application-rest-operands args)
		    ct-env)))))


(define (compile-application-argument-list-continue args ct-env)
    (if (application-no-operand? args)
	the-empty-instruction-sequence
	(instruction-sequence-preserve
	    '(env) ;     "the-empty-instruction-sequence" needs no register,
	           ; therefore "env" will never be "saved" in evaluating any
	           ; final argument.
	    (instruction-sequence-preserve
	        '(argl)
		(compile (application-first-operand args) 'val 'next ct-env)
		(make-instruction-sequence
		    '(argl val)
		    '(argl)
		    '((assign argl (op cons) (reg val) (reg argl)))))
	    (compile-application-argument-list-continue
	        (application-rest-operands args)
		ct-env))))


(define (compile-application-apply target linkage)
    (let ((primitive-procedure-label (make-compiler-label 'apply-primitive-procedure))
	  (compiled-procedure-label (make-compiler-label 'apply-compiled-procedure))
	  (compound-procedure-label (make-compiler-label 'apply-compound-procedure))
	  (done-label (make-compiler-label 'apply-done)))
        (let ((cplinkage (if (eq? linkage 'next)
					   done-label
					   linkage)))
            (instruction-sequence-append
	        (make-instruction-sequence
	            '(proc)
		    '()
		    `((test (op primitive-procedure?) (reg proc))
		      (branch (label ,primitive-procedure-label))
		      (test (op compound-procedure?) (reg proc))
		      (branch (label ,compound-procedure-label))))
		(instruction-sequence-parallel
		    (instruction-sequence-parallel
		        (instruction-sequence-append
		            compiled-procedure-label
		            (compile-application-apply-compiled-procedure
		                target
		                cplinkage))
		        (instruction-sequence-append
		            compound-procedure-label
		            (compile-application-apply-compound-procedure
		                target
		                cplinkage)))
		    (attach-linkage
		        linkage
	                (make-instruction-sequence
	                    '(proc argl)
		            `(,target)
		            `(,primitive-procedure-label
			      (assign ,target (op primitive-procedure-implementation)
				                  (reg proc))
		              (assign ,target (op simple-apply)
				                  (reg ,target) (reg argl))))))
	        done-label))))


(define (compile-application-apply-compiled-procedure target linkage)
    (cond ((and (not (eq? target 'val)) (eq? linkage 'return))
	      (error "COMPILE: compiled procedure returns but target isn't val"))
	  ((and (eq? target 'val) (eq? linkage 'return))
	      (make-instruction-sequence
		  '(proc continue)
		  ALL-COMPILER-REGISTERS
		  '((assign val (op compiled-procedure-entry) (reg proc))
		    (goto (reg val)))))
	  ((and (eq? target 'val) (not (eq? linkage 'return)))
	      (make-instruction-sequence
		  '(proc)
		  ALL-COMPILER-REGISTERS
		  `((assign continue (label ,linkage))
		    (assign val (op compiled-procedure-entry) (reg proc))
		    (goto (reg val)))))
	  (else (let ((after-proc-ret-label
			 (make-compiler-label 'after-procedure-return)))
	            (make-instruction-sequence
		        '(proc)
		        ALL-COMPILER-REGISTERS
		        `((assign continue (label ,after-proc-ret-label))
			  (assign val (op compiled-procedure-entry) (reg proc))
			  (goto (reg val))
			  ,after-proc-ret-label
			  (assign ,target (reg val))
			  (goto (label ,linkage))))))))


(define (compile-application-apply-compound-procedure target linkage)
    (cond ((and (not (eq? target 'val)) (eq? linkage 'return))
	      (error "COMPILE: compiled procedure returns but target isn't val"))
	  ((and (eq? target 'val) (eq? linkage 'return))
	      (make-instruction-sequence
		  '(proc continue)
		  ALL-COMPILER-REGISTERS
		  '((save continue)
		    (goto (reg compapp)))))
	  ((and (eq? target 'val) (not (eq? linkage 'return)))
	      (make-instruction-sequence
		  '(proc)
		  ALL-COMPILER-REGISTERS
		  `((assign continue (label ,linkage))
		    (save continue)
		    (goto (reg compapp)))))
	  (else (let ((after-proc-ret-label
			 (make-compiler-label 'after-procedure-return)))
	            (make-instruction-sequence
		        '(proc)
		        ALL-COMPILER-REGISTERS
		        `((assign continue (label ,after-proc-ret-label))
			  (save continue)
			  (goto (reg compapp))
			  ,after-proc-ret-label
			  (assign ,target (reg val))
			  (goto (label ,linkage))))))))




; SECTION 5: Syntax Procedures
(define (error-expression? exp) (tagged-list? exp 'error))


(define (error-expression-messages exp) (cdr exp))



(define (self-evaluating? exp) (or (number? exp) (string? exp)))



(define (variable? exp) (symbol? exp))



(define (quoted? exp) (tagged-list? exp 'quote))


(define (quoted-text exp) (cadr exp))



(define (assignment? exp) (tagged-list? exp 'set!))


(define (assignment-variable exp) (cadr exp))


(define (assignment-value-expression exp) (caddr exp))


(define (make-assignment var value)
    (list 'set! var value))



(define (definition? exp) (tagged-list? exp 'define))


(define (definition-variable exp)
    (if (variable? (cadr exp))
	(cadr exp)
	(caadr exp)))


(define (definition-value-expression exp)
    (if (variable? (cadr exp))
	(caddr exp)
	(make-lambda (cdadr exp) (cddr exp))))



(define (lambda? exp) (tagged-list? exp 'lambda))


(define (lambda-formal-parameters exp) (cadr exp))


(define (lambda-body exp) (cddr exp))


(define (make-lambda parameters body)
    (cons 'lambda (cons parameters body)))



(define (if? exp) (tagged-list? exp 'if))


(define (if-predicate exp) (cadr exp))


(define (if-consequent-expression exp) (caddr exp))


(define (if-alternative-expression exp)
    (if (null? (cdddr exp))
	'none
	(cadddr exp)))


(define (make-if predicate consequence alternative)
    (list 'if predicate consequence alternative))



(define (or? exp) (tagged-list? exp 'or))


(define (or-predicates exp) (cdr exp))


(define (or-no-predicate? predicates) (null? predicates))


(define (or-first-predicate predicates) (car predicates))


(define (or-rest-predicates predicates) (cdr predicates))



(define (and? exp) (tagged-list? exp 'and))


(define (and-predicates exp) (cdr exp))


(define (and-no-predicate? predicates) (null? predicates))


(define (and-first-predicate predicates) (car predicates))


(define (and-rest-predicates predicates) (cdr predicates))




(define (begin? exp) (tagged-list? exp 'begin))


(define (begin-expressions exp) (cdr exp))


(define (make-begin seq) (cons 'begin seq))


; used in exercise 5.28 for removing tail-recursion
(define (sequence-no-expression? seq) (null? seq))


;     It is needed in implementing tail recursion, the last
; expression are handled specially for this purpose
(define (sequence-last-expression? seq)
    (if (null? seq)
	(error "SEQUENCE: there is no expression")
	(null? (cdr seq))))


(define (sequence-first-expression seq) (car seq))


(define (sequence-rest-expressions seq) (cdr seq))


(define (sequence->exp seq)
    (cond ((null? seq) #t)
	  ((sequence-last-expression? seq)
	      (sequence-first-expression seq))
	  (else (make-begin seq))))


(define (sequence-no-expression? seq) (null? seq))


(define (sequence-append a b) (append a b))


(define (make-sequence . exps) exps)



(define (let? exp) (tagged-list? exp 'let))


(define (let-bindings exp) (cadr exp))


(define (let-parameters exp)
    (map car (let-bindings exp)))


(define (let-arguments exp)
    (map cadr (let-bindings exp)))


(define (let-body exp) (cddr exp))


(define (let->application exp)
    (make-application (make-lambda (let-parameters exp)
				   (let-body exp))
		      (let-arguments exp)))



(define (application? exp) (pair? exp))


(define (application-operator exp) (car exp))


(define (application-operands exp) (cdr exp))


(define (application-no-operand? operands) (null? operands))


(define (application-first-operand operands) (car operands))


(define (application-rest-operands operands) (cdr operands))


(define (application-operand-number operands)
    (length operands))


(define (make-application operator operands)
    (cons operator operands))



(define (cond? exp) (tagged-list? exp 'cond))


(define (cond-clauses exp) (cdr exp))


(define (cond-else-clause? clause) (tagged-list? clause 'else))


(define (cond-no-clause? clauses) (null? clauses))


(define (cond-first-clause clauses) (car clauses))


(define (cond-rest-clauses clauses) (cdr clauses))


(define (cond-clause-predicate clause) (car clause))


(define (cond-clause-expressions clause) (cdr clause))


(define (cond->if exp)
    (define (iter clauses)
        (if (cond-no-clause? clauses)
	    'false
	    (let ((first (cond-first-clause clauses))
		  (rest (cond-rest-clauses clauses)))
	        (if (cond-else-clause? first)
		    (if (cond-no-clause? rest)
			(sequence->exp (cond-clause-expressions first))
			(eceval-error
			    "COND: the else clause is not at the last"))
		    (make-if (cond-clause-predicate first)
			     (sequence->exp (cond-clause-expressions first))
			     (iter rest))))))

    (iter (cond-clauses exp)))



(define (load? exp) (tagged-list? exp 'load))


(define (load-procedure-name exp) (cadr exp))


(define (load-get-procedure-definition name)
    (let ((record (assoc name LOAD-PREDEFINED-PROCEDURES)))
        (if record
	    (cadr record)
	    (eceval-error "LOAD: unexpected predefined procedure name:"
		          name))))




; primitive procedure representation
(define (make-primitive-procedure implementation)
    (list 'primitive-procedure implementation))


(define (primitive-procedure? obj)
    (tagged-list? obj 'primitive-procedure))


(define (primitive-procedure-implementation pproc) (cadr pproc))




; compound procedure representation
(define (make-compound-procedure-generator)
    (let ((cproc-number 0))
        (lambda (parameters body env)
	    (let ((n cproc-number))
	        (set! cproc-number (+ cproc-number 1))
	        (list 'compound-procedure parameters body env n)))))



(define (compound-procedure? obj)
    (tagged-list? obj 'compound-procedure))


(define (compound-procedure-formal-parameters cproc) (cadr cproc))


(define (compound-procedure-body cproc) (caddr cproc))


(define (compound-procedure-environment cproc) (cadddr cproc))


(define (compound-procedure-number cproc) (car (cddddr cproc)))



(define (compound-procedure-print cproc)
    (display "[COMPOUND PROCEDURE #")
    (display (compound-procedure-number cproc))
    (display "]"))




; compiled procedure representation
(define (make-compiled-procedure-generator)
    (let ((cproc-number 0))
        (lambda (proc-entry def-env)
	    (let ((n cproc-number))
	        (set! cproc-number (+ cproc-number 1))
	        (list 'compiled-procedure proc-entry def-env n)))))




(define (compiled-procedure? obj)
    (tagged-list? obj 'compiled-procedure))


(define (compiled-procedure-entry cproc) (cadr cproc))


(define (compiled-procedure-environment cproc) (caddr cproc))


(define (compiled-procedure-number cproc) (cadddr cproc))



(define (compiled-procedure-print cproc)
    (display "[COMPILED PROCEDURE #")
    (display (compiled-procedure-number cproc))
    (display "]"))




; frame representation
(define (make-frame vars vals) (cons vars vals))



(define (frame-variables frame) (car frame))


(define (frame-values frame) (cdr frame))


(define (frame-add-binding frame var val)
    (set-car! frame (cons var (car frame)))
    (set-cdr! frame (cons val (cdr frame))))




; environment representation
(define the-empty-environment '())



(define (environment-null? env) (null? env))


(define (environment-get-enclosing-environment env) (cdr env))


(define (environment-get-first-frame env) (car env))


(define (environment-add-frame env frame) (cons frame env))



(define (environment-extend env vars vals)
    (if (= (length vars) (length vals))
	(environment-add-frame env (make-frame vars vals))
	(eceval-error
	    "ENVIRONMENT EXTEND: variables and values do not match")))


(define (environment-lookup-binding env var)
    (define (find-frame env)
        (define (find-binding vars vals)
	    (cond ((null? vars)
		      (find-frame (environment-get-enclosing-environment env)))
		  ((eq? var (car vars))
                      (car vals))
		  (else (find-binding (cdr vars) (cdr vals)))))

	(if (environment-null? env)
	    (eceval-error "ENVIRONMENT LOOKUP BINDING: unbound variable:" var)
	    (let ((frame (environment-get-first-frame env)))
	        (find-binding (frame-variables frame)
			      (frame-values frame)))))

    (find-frame env))


(define (environment-modify-binding env var new-val)
    (define (find-frame env)
        (define (find-binding vars vals)
	    (cond ((null? vars)
		      (find-frame (environment-get-enclosing-environment env)))
		  ((eq? var (car vars))
                      (set-car! vals new-val))
		  (else (find-binding (cdr vars) (cdr vals)))))

	(if (environment-null? env)
	    (eceval-error "ENVIRONMENT MODIFY BINDING: unbound variable:" var)
	    (let ((frame (environment-get-first-frame env)))
	        (find-binding (frame-variables frame)
			      (frame-values frame)))))

    (find-frame env))


(define (environment-add-binding env var val)
    (let ((frame (environment-get-first-frame env)))
        (define (find-binding vars vals)
            (cond ((null? vars)
	              (frame-add-binding frame var val))
	          ((eq? var (car vars))
	              (set-car! vals val))
	          (else (find-binding (cdr vars) (cdr vals)))))

	(find-binding (frame-variables frame)
		      (frame-values frame))))




; lexical address representation
(define (make-lexical-address fnum dnum)
    (cons fnum dnum))



(define (lexical-address-frame-number address)
    (car address))


(define (lexical-address-displacement-number address)
    (cdr address))



(define (lexical-address-lookup env address)
    (define (find-frame env fnum)
        (if (= fnum 0)
	    (environment-get-first-frame env)
	    (find-frame (environment-get-enclosing-environment env)
			(- fnum 1))))

    (define (find-binding vars vals dnum)
        (if (= dnum 0)
	    (cons (car vars) (car vals))
	    (find-binding (cdr vars)
			  (cdr vals)
			  (- dnum 1))))

    (let ((fnum (lexical-address-frame-number address))
	  (dnum (lexical-address-displacement-number address)))
        (let ((frame (find-frame env fnum)))
	    (let ((binding (find-binding (frame-variables frame)
				         (frame-values frame)
				         dnum)))
	        (if (eq? (cdr binding) '*unassigned*)
		    (error "LEXICAL ADDRESS LOOKUP: unassigned variable:"
			   (car binding))
		    (cdr binding))))))



(define (lexical-address-set! env address new-val)
    (define (find-frame env fnum)
        (if (= fnum 0)
	    (environment-get-first-frame env)
	    (find-frame (environment-get-enclosing-environment env)
			(- fnum 1))))

    (define (modify-binding! vars vals dnum)
        (if (= dnum 0)
	    (set-car! vals new-val)
	    (modify-binding! (cdr vars)
			     (cdr vals)
			     (- dnum 1))))

    (let ((fnum (lexical-address-frame-number address))
	  (dnum (lexical-address-displacement-number address)))
        (let ((frame (find-frame env fnum)))
	    (modify-binding! (frame-variables frame)
				              (frame-values frame)
				              dnum))))



(define (find-variable ct-env var)
    (define (find-frame ct-env fnum)
        (define (do-find-variable vars fnum dnum)
	    (cond ((null? vars)
		      (find-frame (ct-environment-get-enclosing-environment
				      ct-env)
				  (+ fnum 1)))
		  ((eq? var (car vars))
		      (make-lexical-address fnum dnum))
		  (else (do-find-variable (cdr vars)
					  fnum
					  (+ dnum 1)))))

	(if (ct-environment-null? ct-env)
	    #f
	    (let ((ct-frame (ct-environment-get-first-frame ct-env)))
	        (do-find-variable ct-frame fnum 0))))


    (find-frame ct-env 0))




; compile-time environment representation
(define the-empty-ct-environment '())



(define (ct-environment-extend ct-env vars)
    (cons vars ct-env))


(define (ct-environment-null? ct-env)
    (null? ct-env))


(define (ct-environment-get-first-frame ct-env)
    (car ct-env))


(define (ct-environment-get-enclosing-environment ct-env)
    (cdr ct-env))




; instruction sequence representation
(define (make-instruction-sequence needed-regs modified-regs statements)
    (list needed-regs modified-regs statements))


(define the-empty-instruction-sequence
    (make-instruction-sequence '() '() '()))



(define (instruction-sequence-registers-needed iseq)
    (if (symbol? iseq)
        '()
	(car iseq)))


(define (instruction-sequence-registers-modified iseq)
    (if (symbol? iseq)
        '()
	(cadr iseq)))


(define (instruction-sequence-statements iseq)
    (if (symbol? iseq)
        (list iseq)
	(caddr iseq)))



(define (instruction-sequence-register-needed? iseq reg)
    (memq reg (instruction-sequence-registers-needed iseq)))


(define (instruction-sequence-register-modified? iseq reg)
    (memq reg (instruction-sequence-registers-modified iseq)))



; combining operations of instruction sequences
(define (instruction-sequence-simple-append iseq1 iseq2)
    (make-instruction-sequence
        (list-union (instruction-sequence-registers-needed iseq1)
		    (list-difference
		        (instruction-sequence-registers-needed iseq2)
			(instruction-sequence-registers-modified iseq1)))
	(list-union (instruction-sequence-registers-modified iseq1)
		    (instruction-sequence-registers-modified iseq2))
	(append (instruction-sequence-statements iseq1)
		(instruction-sequence-statements iseq2))))


(define (instruction-sequence-append . iseqs)
    (if (null? iseqs)
        the-empty-instruction-sequence
	(instruction-sequence-simple-append
	    (car iseqs)
	    (apply instruction-sequence-append (cdr iseqs)))))


(define (instruction-sequence-preserve regs iseq1 iseq2)
    (if (null? regs)
	(instruction-sequence-append iseq1 iseq2)
	(let ((r (car regs)))
	    (if (and (instruction-sequence-register-modified? iseq1
							      r)
		     (instruction-sequence-register-needed? iseq2
							    r))
		(instruction-sequence-preserve
		    (cdr regs)
		    (make-instruction-sequence
		        (list-union
			    (list r)
		            (instruction-sequence-registers-needed iseq1))
			(list-difference
			    (instruction-sequence-registers-modified iseq1)
			    (list r))
			(cons `(save ,r)
			      (append (instruction-sequence-statements iseq1)
				      `((restore ,r)))))
		    iseq2)
		(instruction-sequence-preserve
		    (cdr regs)
		    iseq1
		    iseq2)))))


(define (instruction-sequence-tack executed non-executed)
   (make-instruction-sequence
       (instruction-sequence-registers-needed executed)
       (instruction-sequence-registers-modified executed)
       (append (instruction-sequence-statements executed)
	       (instruction-sequence-statements non-executed))))


(define (instruction-sequence-parallel iseq1 iseq2)
    (make-instruction-sequence
        (list-union (instruction-sequence-registers-needed iseq1)
		    (instruction-sequence-registers-needed iseq2))
	(list-union (instruction-sequence-registers-modified iseq1)
		    (instruction-sequence-registers-modified iseq2))
	(append (instruction-sequence-statements iseq1)
		(instruction-sequence-statements iseq2))))



(define (print-statements iseq)
    (for-each (lambda (item)
		  (if (not (symbol? item))
		      (display "\t"))
		  (display item)
		  (newline))
	      (instruction-sequence-statements iseq)))




; --- SECTION 6: Examples ---
; A. an interactive scheme compiler
(make-eceval)
(compiler-start)
