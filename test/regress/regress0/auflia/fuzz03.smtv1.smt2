(set-option :incremental false)
(set-info :status sat)
(set-logic QF_AUFLIA)
(declare-fun p1 ((Array Int Int) (Array Int Int) (Array Int Int)) Bool)
(declare-fun v1 () (Array Int Int))
(declare-fun f1 ((Array Int Int) (Array Int Int) (Array Int Int)) (Array Int Int))
(check-sat-assuming ( (let ((_let_0 (store v1 0 0))) (p1 (f1 v1 v1 _let_0) (f1 v1 (f1 v1 _let_0 v1) _let_0) v1)) ))
