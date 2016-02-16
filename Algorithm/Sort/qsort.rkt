#lang racket
(define (qsort num-list)
  (if (= 0 (length num-list))
      num-list
      (let ((two-intervals (partition (cdr num-list) (car num-list))))
        (append (qsort (car two-intervals))
                (list (car num-list))
                (qsort (cdr two-intervals))
                ))))

(define (partition num-list num)
  (if (= 0 (length num-list))
      (cons '() '())
      (let ((first-num (car num-list))
            (parted-intervals (partition (cdr num-list) num)))
        (if (< first-num num)
            (cons (append (car parted-intervals) (list first-num))
                  (cdr parted-intervals))
            (cons (car parted-intervals)
                  (append (cdr parted-intervals) (list first-num)))))
      ))


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