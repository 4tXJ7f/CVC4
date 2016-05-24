(set-logic QF_UFBV)
(declare-sort U 0)
(declare-const x (_ BitVec 1))
(declare-const y (_ BitVec 1))
(declare-const z (_ BitVec 1))
(declare-fun f ((_ BitVec 1)) U)
(assert (not (= (f x) (f y))))
(assert (not (= (f y) (f z))))
(assert (not (= (f x) (f z))))
(set-info :status unsat)
(check-sat)
