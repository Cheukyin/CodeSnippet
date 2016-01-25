;ex2.2
(define (print-point p)
  (newline)
  (display "(")
  (display (x-point p))
  (display ",")
  (display (y-point p))
  (display ")"))

(define (print-segment segment)
  (newline)
  (print-point (start-segment segment))
  (display ",")
  (print-point (end-segment segment)))

(define (average x y)
  (/ (+ x y) 2))

(define (make-segment start-point end-point)
  (cons start-point end-point))
(define (start-segment segment)
  (car segment))
(define (end-segment segment)
  (cdr segment))
(define (midpoint-segment segment)
  (make-point (average (x-point (start-segment segment))
                       (x-point (end-segment segment)))
              (average (y-point (start-segment segment))
                       (y-point (end-segment segment)))))

(define (make-point x-pos y-pos)
  (cons x-pos y-pos))
(define (x-point point)
  (car point))
(define (y-point point)
  (cdr point))

(print-segment (make-segment (make-point 1 1)
                             (make-point 3 3)))
(print-point (midpoint-segment (make-segment 
                                 (make-point 1 1)
                                 (make-point 3 3))))

;ex2.5
(define (number-of-factor-n m n)
  (define (find-facotr-n x number)
    (if (= (remainder x n) 
           0)
      (find-facotr-n (/ x n) 
                     (+ number 1))
      number))
  (find-facotr-n m 0))

(define (cons a b)
  (* (fast-expt 2 a)   ;define in ex1.16
     (fast-expt 3 b)))
(define (car z)
  (number-of-factor-n z 2))
(define (cdr z)
  (number-of-factor-n z 3))

(car (cons 11 5))
(cdr (cons 7 9))

;ex2.6
(define zero
  (lambda (f)
    (lambda (x) x)))

(define (add-1 n)
  (lambda (f) 
    (lambda (x)
      (f ((n f)
          x)))))

(define one
  (lambda (f)
    (lambda (x) (f x))))

(define two
  (lambda (f)
    (lambda (x)
      (f (f x)))))

;addition
(define (add a b)
  (lambda (f)
    (lambda (x)
      ((a f) ((b f) x)))))
;multiplication
(define (mul a b)
  (lambda (f)
    (lambda (x)
      ((a (b f)) x))))
;exponential
(define (exp a b)
  (lambda (f)
    (lambda (x)
      (((a b) f) x))))

(define (f x) (+ x 2))
(((add-1 two) f) 3)
(((add one two) f) 3)
(((mul one two) f) 3)
(((exp two one) f) 3)

;ex2.7
(define (make-interval a b)
  (if (< a b)
    (cons a b)
    (cons b a)))
(define (upper-bound interval)
  (cdr interval))
(define (lower-bound interval)
  (car interval))

;ex2.8
(define (sub-interval interval1 interval2)
  (make-interval (- (upper-bound interval1)
                    (lower-bound interval2))
                 (- (lower-bound interval1)
                    (upper-bound interval2))))
(+ 1 1)
;ex2.17
(define (last-pair list-given)
  (define (last-element list-given result)
    (if (null? list-given)
      result
      (last-element (cdr list-given) (car list-given))))
  (last-element list-given 0))

(last-pair (list 3 4 5 6))

;ex2.18
(define (reverse list-given)
  (define (reverse-iter lst result)
    (if (null? lst)
      result
      (reverse-iter (cdr lst)
                    (cons (car lst) result))))
  (reverse-iter list-given '()))
(reverse (list 1 2 3 4 5 6 7 8 9))

;ex2.19
(define (cc amount coin-values)
  (cond ((= amount 0) 1)
        ((or (< amount 0)
             (null? coin-values))
         0)
        (else
          (+ (cc amount
                 (cdr coin-values))
             (cc (- amount (car coin-values))
                 coin-values)))))
(define us-coins
  (list 50 25 10 5 1))

(cc 100 us-coins)

;ex2.20
(define (same-parity x . y)
  (define (same-parity-iter parity? lst)
    (cond ((null? lst) '())
          ((parity? (car lst))
           (cons (car lst)
                 (same-parity-iter parity? (cdr lst))))
          (else (same-parity-iter parity? (cdr lst)))))
  (cons x (same-parity-iter  (if (even? x)
                                even?
                                odd?)
                            y)))
(same-parity 8 2 3 4 5 6 7)

;ex2.30
(define (square-tree tree)
  (cond ((null? tree) ())
        ((not (pair? tree)) (* tree tree))
        (else (cons (square-tree (car tree))
                    (square-tree (cdr tree))))))
(square-tree
 (list 1
       (list 2
             (list 3 4)
             5)) )

;ex2.32
(define (subsets s)
    (if (null? s)
        (list '())
        (let ((rest (subsets (cdr s))))
            (append rest (map (lambda (x)
                                (cons (car s) x))
                              rest)))))
(subsets (list 1 2 3))

;ex2.41
(define (flatmap proc seq)
  (accumulate append () (map proc seq)))

(define (accumulate proc initial seq)
  (if (null? seq)
      initial
      (proc (car seq)
          (accumulate proc initial (cdr seq)))))

;generate a list of (start start+1 ... end)
(define (enumerate-interval start end)
  (define (enumerate-interval-iter result current)
    (if (= current end)
        (append result (list current))
        (enumerate-interval-iter (append result (list current))
                                 (+ current 1))))
  (enumerate-interval-iter () start))

(enumerate-interval 1 6)

;generate an n tuple
(define (n-tuple n seq)
  (define (remove i s)
    (filter (lambda (x) (not (= x i)))
            s))
  (if (= n 0)
      (list ())
      (flatmap (lambda (i)
                 (map (lambda (n-1-tuple)
                        (cons i n-1-tuple))
                      (n-tuple (- n 1) (remove i seq))))
               seq)))

(n-tuple 2 (list 1 2 3 4))

(define (triad-of-sum-s n s)
  (filter (lambda (triad)
            (= (+ (car triad)
                  (cadr triad)
                  (caddr triad))
               s))
          (n-tuple 3 (enumerate-interval 1 n))))

(triad-of-sum-s 4 9)

;ex2.42
(define (queens board-size)
  (define (safe? k positions)
    (define (safe-iter pos-before-k parallel up down)
      (cond ((null? pos-before-k) #t)
            ((= (car pos-before-k) parallel) #f)
            ((= (car pos-before-k) up) #f)
            ((= (car pos-before-k) down) #f)
            (else (safe-iter (cdr pos-before-k)
                             parallel
                             (- up 1)
                             (+ down 1)))))
    (safe-iter (cdr positions)
               (car positions)
               (- (car positions) 1)
               (+ (car positions) 1)))
  (define (queen-cols k)
    (if (= k 0)
        (list ())
        (filter (lambda (positions) (safe? k positions))
                (flatmap (lambda (rest-of-queens)
                           (map (lambda (new-row)
                                  (cons new-row rest-of-queens))
                                (enumerate-interval 1 board-size)))
                         (queen-cols (- k 1))))))
  (queen-cols board-size))

(queens 8)

;ex2.57 & ;ex2.58
;check if symbol v1==v2
(define (same-variable? v1 v2)
  (and (symbol? v1) (symbol? v2) (eq? v1 v2)))

;construct an expression (a1 op a2)
(define (construction op a1 a2)
  (list op a1 a2))

;select the first operand
(define (select-first-operand expression)
  (cadr expression))

;select the later operand
(define (select-later-operand expression)
  (if (= 1 (length (cddr expression)))
      (caddr expression)
      (cons (car expression) (cddr expression))))

;judge if the operator of the expression equals to the judger
(define (judge-kind-of-operator judger expression)
  (and (pair? expression) (eq? (car expression) judger)))

;derivation
(define (deriv expression var)
  (cond ((number? expression) 0)
        ((symbol? expression)
         (if (same-variable? expression var)
             1
             0))
        ((judge-kind-of-operator '+ expression)
         (construction '+
                       (deriv (select-first-operand expression)
                              var)
                       (deriv (select-later-operand expression)
                              var)))
        ((judge-kind-of-operator '* expression)
         (construction '+
                       (construction '*
                                     (select-later-operand expression)
                                     (deriv (select-first-operand expression)
                                            var))
                       (construction '*
                                     (select-first-operand expression)
                                     (deriv (select-later-operand expression)
                                            var))))
        ((judge-kind-of-operator '^ expression)
         (construction '*
                       (deriv (select-first-operand expression)
                              var)
                       (construction '*
                                     (select-later-operand expression)
                                     (construction '^
                                                   (select-first-operand expression)
                                                   (- (select-later-operand expression)
                                                      1)))))))

(deriv '(* x y z) 'x)

;ex2.59
(define (element-of-set? x set)
  (cond ((null? set) #f)
        ((equal? x (car set)) #t)
        (else (element-of-set? x (cdr set)))))

(define (union-set set1 set2)
  (if (null? set2)
      set1
      ((union-set (if (element-of-set? (car set2) set1)
                      (cons (car set2) set1)
                      set1)
                  (cdr set2)))))