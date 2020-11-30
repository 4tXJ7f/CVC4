(set-option :incremental false)
(set-info :status sat)
(set-logic QF_AUFBV)
(declare-fun v0 () (_ BitVec 11))
(declare-fun v1 () (_ BitVec 2))
(declare-fun v2 () (_ BitVec 9))
(declare-fun a3 () (Array (_ BitVec 9) (_ BitVec 10)))
(declare-fun a4 () (Array (_ BitVec 9) (_ BitVec 14)))
(declare-fun a5 () (Array (_ BitVec 13) (_ BitVec 4)))
(check-sat-assuming ( (let ((_let_0 (bvnand (_ bv28452 16) ((_ zero_extend 7) v2)))) (let ((_let_1 (bvurem ((_ sign_extend 7) v1) v2))) (let ((_let_2 (ite (bvult ((_ zero_extend 7) v2) _let_0) (_ bv1 1) (_ bv0 1)))) (let ((_let_3 (bvmul ((_ sign_extend 14) v1) (_ bv28452 16)))) (let ((_let_4 ((_ sign_extend 2) v2))) (let ((_let_5 (select a4 ((_ extract 9 1) (select a3 ((_ extract 8 0) v0)))))) (let ((_let_6 (select a3 ((_ extract 11 3) _let_3)))) (let ((_let_7 (ite (bvsge _let_6 ((_ zero_extend 8) v1)) (_ bv1 1) (_ bv0 1)))) (let ((_let_8 (bvsmod ((_ zero_extend 6) _let_6) _let_3))) (let ((_let_9 ((_ repeat 1) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0))))))) (let ((_let_10 (bvurem ((_ sign_extend 12) v1) _let_9))) (let ((_let_11 (ite (bvslt ((_ sign_extend 2) _let_10) _let_8) (_ bv1 1) (_ bv0 1)))) (let ((_let_12 ((_ zero_extend 1) _let_2))) (let ((_let_13 ((_ zero_extend 3) v0))) (let ((_let_14 ((_ sign_extend 15) (ite (bvult ((_ sign_extend 9) (bvcomp v0 _let_4)) (select a3 ((_ extract 8 0) v0))) (_ bv1 1) (_ bv0 1))))) (let ((_let_15 (bvugt (ite (distinct (_ bv28452 16) ((_ sign_extend 2) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))))) (_ bv1 1) (_ bv0 1)) (ite (bvult _let_5 _let_13) (_ bv1 1) (_ bv0 1))))) (let ((_let_16 ((_ sign_extend 8) (ite (distinct (_ bv28452 16) ((_ sign_extend 2) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))))) (_ bv1 1) (_ bv0 1))))) (let ((_let_17 (or (bvuge _let_7 (bvcomp v0 _let_4)) (or (ite (bvslt ((_ sign_extend 10) (ite (bvult ((_ sign_extend 9) (bvcomp v0 _let_4)) (select a3 ((_ extract 8 0) v0))) (_ bv1 1) (_ bv0 1))) v0) (ite (= ((_ zero_extend 2) _let_5) _let_0) (= _let_1 _let_16) (and (bvsgt ((_ zero_extend 15) (bvcomp v0 _let_4)) (bvnand ((_ sign_extend 6) (select a3 ((_ extract 8 0) v0))) _let_0)) (bvslt (ite (bvult _let_5 _let_13) (_ bv1 1) (_ bv0 1)) _let_11))) (bvugt _let_13 _let_9)) (and (ite (or (bvuge (ite (bvult _let_5 _let_13) (_ bv1 1) (_ bv0 1)) (ite (bvult _let_5 _let_13) (_ bv1 1) (_ bv0 1))) (or (bvsgt ((_ sign_extend 8) _let_2) (bvand _let_1 ((_ zero_extend 8) _let_2))) (bvult _let_10 ((_ sign_extend 3) (bvxor _let_4 v0))))) (bvsge (ite (bvult _let_5 _let_13) (_ bv1 1) (_ bv0 1)) (ite (bvuge _let_12 v1) (_ bv1 1) (_ bv0 1))) (bvuge _let_6 ((_ sign_extend 9) (ite (distinct (_ bv28452 16) ((_ sign_extend 2) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))))) (_ bv1 1) (_ bv0 1))))) (or (xor (= (bvslt (_ bv28452 16) _let_14) (bvugt (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))) ((_ sign_extend 13) (ite (bvult ((_ sign_extend 9) (bvcomp v0 _let_4)) (select a3 ((_ extract 8 0) v0))) (_ bv1 1) (_ bv0 1))))) (bvult _let_3 ((_ sign_extend 2) _let_10))) (not (bvsle _let_16 _let_1)))))))) (let ((_let_18 (ite (= (bvule _let_6 (select a3 ((_ extract 8 0) v0))) (= ((_ sign_extend 12) v1) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))))) (ite (or (= (bvsge _let_1 ((_ zero_extend 8) _let_7)) (bvuge _let_11 _let_7)) (= (bvsge _let_1 ((_ zero_extend 8) _let_7)) (bvuge _let_11 _let_7))) (bvsge ((_ zero_extend 6) (select a3 ((_ extract 8 0) v0))) _let_3) (not (or (= (select a3 ((_ extract 8 0) v0)) ((_ sign_extend 9) (ite (distinct (_ bv28452 16) ((_ sign_extend 2) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))))) (_ bv1 1) (_ bv0 1)))) (= (select a3 ((_ extract 8 0) v0)) ((_ sign_extend 9) (ite (distinct (_ bv28452 16) ((_ sign_extend 2) (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))))) (_ bv1 1) (_ bv0 1))))))) (= (distinct ((_ zero_extend 13) (ite (bvult _let_5 _let_13) (_ bv1 1) (_ bv0 1))) _let_5) (bvule _let_2 _let_2))))) (let ((_let_19 (ite (=> (not (bvugt ((_ sign_extend 3) (bvmul v0 v0)) _let_5)) (bvuge ((_ sign_extend 14) v1) (_ bv28452 16))) (or (bvugt ((_ sign_extend 15) (bvcomp v0 _let_4)) _let_0) (bvugt ((_ sign_extend 15) (bvcomp v0 _let_4)) _let_0)) (or (bvugt ((_ sign_extend 15) (bvcomp v0 _let_4)) _let_0) (bvugt ((_ sign_extend 15) (bvcomp v0 _let_4)) _let_0))))) (let ((_let_20 (not (or (or (ite (bvsge ((_ sign_extend 13) (bvcomp v0 _let_4)) _let_5) (distinct (bvand _let_1 ((_ zero_extend 8) _let_2)) ((_ zero_extend 8) _let_7)) (bvsgt _let_0 _let_8)) (bvuge (ite (bvult ((_ sign_extend 9) (bvcomp v0 _let_4)) (select a3 ((_ extract 8 0) v0))) (_ bv1 1) (_ bv0 1)) (ite (bvuge _let_12 v1) (_ bv1 1) (_ bv0 1)))) (or (ite (bvsge ((_ sign_extend 13) (bvcomp v0 _let_4)) _let_5) (distinct (bvand _let_1 ((_ zero_extend 8) _let_2)) ((_ zero_extend 8) _let_7)) (bvsgt _let_0 _let_8)) (bvuge (ite (bvult ((_ sign_extend 9) (bvcomp v0 _let_4)) (select a3 ((_ extract 8 0) v0))) (_ bv1 1) (_ bv0 1)) (ite (bvuge _let_12 v1) (_ bv1 1) (_ bv0 1)))))))) (and (and (and (and (=> (and _let_19 _let_19) (not (= (= _let_20 _let_20) (and (= _let_17 _let_17) (ite _let_18 _let_18 (=> (= (and (=> (xor (bvult _let_12 v1) (and (bvuge ((_ zero_extend 2) _let_9) _let_3) (and (= ((_ sign_extend 1) _let_11) v1) (bvslt _let_8 ((_ zero_extend 15) _let_7))))) (=> _let_15 _let_15)) (not (distinct _let_14 (_ bv28452 16)))) (ite (bvsle _let_4 v0) (bvule _let_14 (_ bv28452 16)) (not (bvule ((_ sign_extend 2) _let_5) _let_0)))) (= (bvsle (select a4 ((_ extract 8 0) (select a3 ((_ extract 8 0) v0)))) ((_ zero_extend 5) _let_1)) (bvule _let_10 _let_13)))))))) (not (= v2 (_ bv0 9)))) (not (= _let_9 (_ bv0 14)))) (not (= _let_3 (_ bv0 16)))) (not (= _let_3 (bvnot (_ bv0 16)))))))))))))))))))))))))) ))
