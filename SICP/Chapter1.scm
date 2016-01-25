;ex1.10
(define (A x y)
  (cond ((= y 0) 0)
        ((= x 0) (* 2 y))
        ((= y 1) 2)
        (else (A (- x 1)
                 (A x (- y 1))))))

(define (f n) (A 0 n))
(f 8)

;ex1.11
(define (func-recursive n)
  ( cond ((< n 3) n)
         (else (+ (func-recursive (- n 1))
                  (* 2 (func-recursive (- n 2)))
                  (* 3 (func-recursive (- n 3)))))))
(func-recursive 5)

(define (func_iter n)
  (define (func-iter a b c i)
    (cond ((= n 0) c)
          ((= n 1) b)
          ((= n 2) a)
          ((= n i) a)
          (else (func-iter (+ a (* 2 b) (* 3 c))
                           a b
                           (+ i 1)))))
  (func-iter 2 1 0 2))
(func_iter 5)

;ex1.16
(define (fast-expt a y)
  (define (expt-iter b n a)
    (define (double a)
      (* a a))
    (define (even? n)
      (= (remainder n 2) 0))
    (cond ((= n 0)
           a)
          ((even? n)
           (expt-iter (double b)
                      (/ n 2)
                      a))
          (else
            (expt-iter b
                       (- n 1)
                       (* b a)))))
  (expt-iter a y 1))
(fast-expt 2 7)

;ex1.17
(define (mul_recursive a b)
  (define (double a)
    (+ a a))
  (define (half a)
    (/ a 2))
  (define (even? n)
    (= (remainder n 2) 0))
  (cond ((= b 0) 0)
        ((even? b) (double (mul_recursive a (half b))))
        (else (+ a (mul_recursive a (- b 1))))))
(mul_recursive 3 90)

;ex1.18
(define (mul_iter a b)
  (define (double a)
    (+ a a))
  (define (half a)
    (/ a 2))
  (define (even? n)
    (= (remainder n 2) 0))
  (define (mul-iter a b result)
    (cond ((= b 0) result)
          ((even? b) (mul-iter (double a) (half b) result))
          (else (mul-iter a (- b 1) (+ a result)))))
  (mul-iter a b 0))
(mul_iter 10 110)

;ex1.22 & 1.23
(define (smallest-divisor n)
  (find-divisor n 2))
(define (find-divisor n test-divisor)
  (define (next t)
    (if (= t 2)
      3
      (+ t 2)))
  (cond ((> (square test-divisor) n) n)
        ((divides? test-divisor n) test-divisor)
        (else (find-divisor n (next test-divisor)))))
(define (divides? a b)
  (= (remainder b a) 0))
(define (prime? n)
  (= n (smallest-divisor n)))

(define (search-for-primes n start-time)
  (define (even? i)
    (= (remainder i 2) 0))
  (define (search-odd-primes i)
    (cond ((prime? i)
           (display i)
           (newline)
           (display (- (real-time-clock) start-time)))
          (else (search-odd-primes (+ i 2)))))
  (if (even? n)
    (search-odd-primes (+ n 1))
    (search-odd-primes n)))
(search-for-primes 1000000000 (real-time-clock))

;ex1.24 & 1.28
(define (expmod base exp m)
  (define (nontrivial-square-root? a b)
    (and (not (= a 1))                      ;1.28
         (not (= a (- b 1)))                ;1.28
         (= (remainder (square a) b) 1)))   ;1.28
  (cond ((= exp 0) 1)
        ((nontrivial-square-root? base m) 0)
        ((even? exp)
         (remainder (square (expmod base (/ exp 2) m))
                    m))
        (else (remainder (* base
                            (expmod base
                                    (- exp 1)
                                    m))
                         m))))
(define (fermat-test n)
  (define (try-it a)
    (= (expmod a n n) a))
  (try-it (+ 1 (random(- n 1)))))
(define (prime? n)
  (define (fast-prime? n times)
    (cond ((= n 1) false)
          ((= times 0) true)
          ((fermat-test n) (fast-prime? n (- times 1)))
          (else false)))
  (fast-prime? n 4))

(search-for-primes 561 (real-time-clock))

;ex1.29
(define (sum term a next b)
  (if (> a b)
    0
    (+ (term a)
       (sum term (next a) next b))))

(define (simpson-integral f a b n)
  (define h
    (/ (- b a) n))

  (define (para k)
    (+ a (* k h)))

  (define (func k)
    (cond ((= k 0) (f a))
          ((= k n) (f b))
          ((even? k)
           (* 2 
              (f (para k))))
          (else (* 4 
                   (f (para k))))))

  (define (next-k x)
    (+ x 1))

  (* (/ h 3.0)
     (sum func (exact->inexact 0) next-k n)))

(define (cube x)
  (* x x x))
(simpson-integral cube  0 1 1000000)

;ex1.30
(define (sum term a next b)
  (define (sum-iter current-a result)
    (if (> current-a b)
      result
      (sum-iter (next current-a) (+ result (term current-a)))))
  (sum-iter a 0))

;ex1.31
;(1)
(define (product term a next b)
  (if (> a b)
    1
    (* (term a)
       (product term (next a) next b))))

(define (factorial n)
  (define (next a)
    (+ a 1))
  (define (term a)
    a)
  (product term 1 next n))
(factorial 5)

(define (pi/4 n)
  (define (son a)
    (cond ((even? a) a)
          (else (+ a 1))))
  (define (mother a)
    (cond ((even? a) (+ a 1))
          (else a)))
  (define (term a)
    (/ (son a) (mother a)))
  (define (next a)
    (+ a 1))
  (exact->inexact (product term 2 next (+ 1 n))))
(* 4 (pi/4 10000))
;(2)
(define (product term a next b)
  (define (product-iter current-a result)
    (if (> current-a b)
      result
      (product-iter (next current-a) (* result (term current-a)))))
  (product-iter a 1))

;ex1.32
;(a)
(define (accumulate combiner null-value term a next b)
  (if (> a b)
    null-value
    (combiner (term a)
       (accumulate combiner null-value term (next a) next b))))

(define (sum term a next b)
  (accumulate + 0 term a next b))
(define (product term a next b)
  (accumulate * 1 term a next b))
;(b)
(define (accumulate combiner null-value term a next b)
  (define (accumulate-iter current-a result)
    (if (> current-a b)
      null-value
      (accumulate-iter (next current-a) (combiner result (term current-a)))))
  (accumulate-iter a null-value))

;ex1.33
(define (filtered-accumulate filter? combiner null-value term a next b)
  (cond ((> a b) null-value)
        ((filter? a) 
         (combiner (term a) 
                   (filtered-accumulate filter? combiner null-value term (next a) next b)))
        (else 
          (filtered-accumulate filter? combiner null-value term (next a) next b))))

;(a)
(define (primes-sum a b)
  (define (term a)
    a)
  (define (next a)
    (+ a 1))
  (filtered-accumulate prime? + 0 term a next b))
(primes-sum 1 11)

;(b)
(define (product-coprimes n)
  (define (coprimes? i)
    (if (= 1 (gcd i n))
      true
      false))
  (define (term a)
    a)
  (define (next a)
    (+ a 1))
  (filtered-accumulate coprimes? * 1 term 1 next n))
(product-coprimes 10)

;ex1.36
(define tolerance 0.00001)
(define (fixed-point f first-guess)
  (define (close-enough? v1 v2)
    (< (abs (- v1 v2)) tolerance))
  (define (try guess step)
    (let ((next (f guess)))
      (cond ((close-enough? guess next) next)
            (else 
              (display step)
              (display ".")
              (display next)
              (newline)
              (try next (+ step 1))))))
  (try first-guess 1))

(fixed-point (lambda (x) (/ (log 1000) (log x))) 4)
;average-damp
(fixed-point (lambda (x) (/ (+ x (/ (log 1000) (log x))) 2)) 4)

;ex1.37
;(a)
;recursion
(define (cont-factorial n d k)
  (cond ((= k 0) 0)
        (else (/ (n k)
                 (+ (d k)
                    (cont-factorial n d (- k 1)))))))
(/ 1 (cont-factorial (lambda (i) 1.0) (lambda (i) 1.0) 10003))
;(b)
;iteration
(define (cont-factorial n d k)
  (define (cont-factorial-iter result i)
    (cond ((= i 1) (/ (n 1) (+ (d 1) result)))
          (else 
            (cont-factorial-iter (/ (n k) 
                                    (+ result 
                                       (d k)))
                                 (- i 1)))))
  (cont-factorial-iter 0 k))

;ex1.38
(define (d i)
  (cond ((= 2 (remainder i 3)) 
         (* (/ 2 3) 
            (+ i 1)))
         (else 1.0)))
(+ 2.0 (cont-factorial (lambda (i) 1.0) d 12))

;ex1.39
(define (tan-cf x k)
  (define (d i)
    (- (* 2 i) 1))
  (define (n i)
    (cond ((= i 1) x)
          (else (- (square x)))))
  (cont-factorial n d k))
(tan-cf 10.0 3)
(tan 10)

;ex1.40
(define (deriv g)
  (define dx 0.00001)
  (lambda (x)
    (/ (- (g (+ x dx)) 
          (g x)) 
       dx)))

(define (newton-transform g)
  (lambda (x)
    (- x
       (/ (g x)
          ((deriv g) x)))))

(define (newton-method g guess)
  (fixed-point (newton-transform g) guess))

(define (cubic a b c)
  (define (cube x)
    (* x x x))
  (lambda (x)
    (+ (cube x)
       (* a (square x))
       (* b x)
       c)))
(newton-method (cubic 3 2 1) 1.0)

;ex1.41
(define (inc x)
  (+ x 1))
(define (double f)
  (lambda (x)
    (f (f x))))
(((double (double double)) inc) 16)

;ex1.42
(define (compose f g)
  (lambda (x)
    (f (g x))))
((compose square inc) 6)

;ex1.43
(define (repeat f n)
  (cond ((= n 1) (lambda (x) (f x)))
        (else (compose f (repeat f (- n 1))))))
((repeat square 2) 5)

;ex1.44
(define (smooth f n)
  (define dx 0.00001)
  (define (smooth-init func)
    (lambda (x)
      (/ (+ (func (- x dx))
            (func x)
            (func (+ x dx)))
         3)))
  ((repeat smooth-init n) f))
((smooth square 10) 5)

;ex1.45
(define (average x y)
  (/ (+ x y) 2))

(define (power y m)
  (cond ((= m 0) 1)
        (else (* y (power y (- m 1))))))

(define (nth-average-damp f n)
  (define (average-damp f)
    (lambda (x)
      (average x (f x))))
  ((repeat average-damp n) f))

(define (mth-root x m)
  (fixed-point (nth-average-damp (lambda (y)
                                   (/ x (power y (- m 1))))
                                 (floor (/ (log m) (log 2))))
               1.0))

(mth-root 4 28)

;ex1.46
(define (iterative-improve good-enough? improve-method)
  (define (try guess)
    (let ((next (improve-method guess)))
      (cond ((good-enough? guess next)
             next)
            (else (try next)))))
  try)

(define (sqrt x guess)
  (define tolerance 0.00001)
  (define (good-enough? v1 v2)
    (< (abs (- v1 v2)) tolerance))
  ((iterative-improve good-enough? 
                     (lambda (guess) (average guess (/ x guess))))
   guess))

(define (fixed-point f guess)
  (define tolerance 0.00001)
  (define (good-enough? v1 v2)
    (< (abs (- v1 v2)) tolerance))
  ((iterative-improve good-enough? 
                     (nth-average-damp f 1))
   guess))

(sqrt 2 1.0)
(fixed-point cos 1.0)