; COMMAND-LINE: --cbqi-bv --bv-div-zero-const
; EXPECT: sat
(set-logic BV)
(set-info :status sat)
(declare-fun a () (_ BitVec 4))
(declare-fun b () (_ BitVec 4))

(assert (forall ((x (_ BitVec 4))) (not (= (bvurem x a) b))))

(check-sat)
