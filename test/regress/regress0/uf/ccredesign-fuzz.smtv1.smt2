(set-option :incremental false)
(set-info :status sat)
(set-logic QF_UF)
(declare-sort U 0)
(declare-sort S1 0)
(declare-sort S0 0)
(declare-fun v0 () S0)
(declare-fun v1 () S0)
(declare-fun f4 (S1) S1)
(declare-fun v2 () S1)
(declare-fun v3 () S1)
(declare-fun p3 (S0 S0 S1) Bool)
(declare-fun f0 (S0 S0 S1) S0)
(declare-fun p1 (S0 S1 S1) Bool)
(declare-fun p4 (S1) Bool)
(check-sat-assuming ( (let ((_let_0 (ite (= v3 (f4 v2)) v1 v0))) (ite (p4 v3) (= (p4 (f4 v2)) (p3 (ite (p1 v0 (f4 v2) (f4 v2)) (f0 v1 v1 v2) v0) v0 (f4 v2))) (=> (=> (distinct _let_0 _let_0) false) false))) ))
