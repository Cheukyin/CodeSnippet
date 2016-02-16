#lang racket
(define (qsort num-list)
  (match num-list
    ['()        '()]
    [(cons n l) (append (qsort (filter (λ (x) (< x n)) l))
                        (list n)
                        (qsort (filter (λ (x) (>= x n)) l)))]))


(require rackunit rackunit/text-ui)

(define QSORT-TESTS
  (test-suite "Tests For Qsort"
              (test-case "qsort"
                         (check-equal? (qsort '())
                                       '())
                         (check-equal? (qsort '(1))
                                       '(1))
                         (check-equal? (qsort '(9 8 7 6 5 4 3 8 2 1))
                                       '(1 2 3 4 5 6 7 8 8 9))
                         (check-equal? (qsort '(2 2 2 2 2))
                                       '(2 2 2 2 2))
                         )))


(run-tests QSORT-TESTS)