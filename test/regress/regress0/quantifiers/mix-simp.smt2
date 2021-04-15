; COMMAND-LINE: --no-check-models
; EXPECT: sat
(set-logic UFLIRA)
(set-info :status sat)
(assert (forall ((x Real)) (exists ((y Int)) (and (>= y x) (< y (+ x 1))))))
(check-sat)
