; EXPECT: sat
; EXPECT: not declared
; SCRUBBER: grep -o "sat\|not declared"
; EXIT: 1
(set-logic QF_BVLIA)
(declare-const x (_ BitVec 4))
(assert (= (bv2nat x) 0))
(check-sat)
(reset)
(set-logic QF_BV)
(declare-const x (_ BitVec 4))
(assert (= (bv2nat x) 0))
(check-sat)
