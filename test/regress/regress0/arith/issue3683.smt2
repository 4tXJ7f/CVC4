(set-logic ALL)
(declare-fun a () Real)
(assert (= (+ 2 (exp (+ 2 a))) 0))
(set-info :status unsat)
(check-sat)
