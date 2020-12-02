; SCRUBBER: sed -e 's/IP_[0-9]*/IP/'
; EXPECT: unsat
; EXPECT: (
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: IP
; EXPECT: )
(set-logic ALL)
(set-option :produce-unsat-cores true)
(declare-const v0 Bool)
(declare-const v1 Bool)
(declare-const v2 Bool)
(declare-const v3 Bool)
(declare-const v4 Bool)
(declare-const i1 Int)
(declare-const r1 Real)
(declare-const r2 Real)
(declare-const r3 Real)
(declare-const r7 Real)
(declare-const r10 Real)
(declare-const r14 Real)
(declare-const r16 Real)
(declare-const r17 Real)
(declare-const r18 Real)
(declare-const v5 Bool)
(declare-const r20 Real)
(declare-const arr-5074471134850140881_778238581772319464-0 (Array Bool Int))
(assert (! (exists ((q0 Int) (q1 Real) (q2 Int)) (not (> (- i1) (select (store arr-5074471134850140881_778238581772319464-0 v0 60) (= v0 v4 v3 v3 v4))))) :named IP_1))
(assert (! (or (forall ((q0 Int) (q1 Real) (q2 Int)) (=> (>= r20 q1 q1 26271062.0) (<= q2 q2))) (exists ((q0 Int) (q1 Real) (q2 Int)) (=> (>= (- i1) q2) (< q1 q1 5851734.0)))) :named IP_2))
(declare-const arr-778238581772319464_-3713212254555730767-0 (Array Int (_ BitVec 8)))
(declare-const v6 Bool)
(declare-const v7 Bool)
(declare-const arr--5101604640448673848_-3713212254554648242-0 (Array Real (_ BitVec 9)))
(declare-const arr--3713212254557895817_-5101604640448673848-0 (Array (_ BitVec 10) Real))
(assert (! (or (distinct (- i1) 35) (not v4) (= v0 v4 v3 v3 v4)) :named IP_3))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v0 (= v0 v4 v3 v3 v4)) :named IP_4))
(assert (! (or (= v0 v4 v3 v3 v4) (distinct v4 (= v0 v4 v3 v3 v4)) (not v4)) :named IP_5))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v3 (= v0 v4 v3 v3 v4)) :named IP_6))
(assert (! (or (= v0 v4 v3 v3 v4) v3 (= v0 v4 v3 v3 v4)) :named IP_7))
(assert (! (or (is_int 1193124502.0) (= v0 v4 v3 v3 v4) (not v4)) :named IP_8))
(assert (! (or (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4) v0) :named IP_9))
(assert (! (or (< 397 (div (- i1) 60)) v3 (= v0 v4 v3 v3 v4)) :named IP_10))
(assert (! (or v3 v3 v3) :named IP_11))
(assert (! (or (is_int 1193124502.0) (not v4) (= v0 v4 v3 v3 v4)) :named IP_12))
(assert (! (or (< 397 (div (- i1) 60)) (is_int 1193124502.0) (distinct (- i1) 35)) :named IP_13))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) v0) :named IP_14))
(assert (! (or (is_int 1193124502.0) v0 (not v4)) :named IP_15))
(assert (! (or (< 397 (div (- i1) 60)) (< 397 (div (- i1) 60)) (< 397 (div (- i1) 60))) :named IP_16))
(assert (! (or (not v4) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35)) :named IP_17))
(assert (! (or (distinct (- i1) 35) (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_18))
(assert (! (or v3 (distinct v4 (= v0 v4 v3 v3 v4)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_19))
(assert (! (or (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4) (is_int 1193124502.0)) :named IP_20))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_21))
(assert (! (or (< 397 (div (- i1) 60)) (distinct (- i1) 35) (= v0 v4 v3 v3 v4)) :named IP_22))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v3 (is_int 1193124502.0)) :named IP_23))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) (= v0 v4 v3 v3 v4)) :named IP_24))
(assert (! (or (= v0 v4 v3 v3 v4) (is_int 1193124502.0) (< 397 (div (- i1) 60))) :named IP_25))
(assert (! (or v0 v0 (= v0 v4 v3 v3 v4)) :named IP_26))
(assert (! (or v0 (not v4) (= v0 v4 v3 v3 v4)) :named IP_27))
(assert (! (or (= v0 v4 v3 v3 v4) v3 v3) :named IP_28))
(assert (! (or v3 (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_29))
(assert (! (or (is_int 1193124502.0) (is_int 1193124502.0) (distinct (- i1) 35)) :named IP_30))
(assert (! (or (distinct (- i1) 35) (not v4) (distinct (- i1) 35)) :named IP_31))
(assert (! (or (= v0 v4 v3 v3 v4) (< 397 (div (- i1) 60)) v3) :named IP_32))
(assert (! (or (= v0 v4 v3 v3 v4) (distinct (- i1) 35) (not v4)) :named IP_33))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v0 v0) :named IP_34))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) (< 397 (div (- i1) 60))) :named IP_35))
(assert (! (or (= v0 v4 v3 v3 v4) (is_int 1193124502.0) (not v4)) :named IP_36))
(assert (! (or (= v0 v4 v3 v3 v4) (distinct (- i1) 35) v0) :named IP_37))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (< 397 (div (- i1) 60)) (< 397 (div (- i1) 60))) :named IP_38))
(assert (! (or (distinct (- i1) 35) v3 (= v0 v4 v3 v3 v4)) :named IP_39))
(assert (! (or (< 397 (div (- i1) 60)) (is_int 1193124502.0) (not v4)) :named IP_40))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v3 (is_int 1193124502.0)) :named IP_41))
(assert (! (or v0 (distinct (- i1) 35) v0) :named IP_42))
(assert (! (or (not v4) v0 (not v4)) :named IP_43))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (not v4) (= v0 v4 v3 v3 v4)) :named IP_44))
(assert (! (or (is_int 1193124502.0) (distinct v4 (= v0 v4 v3 v3 v4)) (is_int 1193124502.0)) :named IP_45))
(assert (! (or v0 (is_int 1193124502.0) (distinct (- i1) 35)) :named IP_46))
(assert (! (or (= v0 v4 v3 v3 v4) v0 (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_47))
(assert (! (or (is_int 1193124502.0) v0 (= v0 v4 v3 v3 v4)) :named IP_48))
(assert (! (or v0 (< 397 (div (- i1) 60)) v3) :named IP_49))
(assert (! (or v0 v3 (is_int 1193124502.0)) :named IP_50))
(assert (! (or v3 (distinct (- i1) 35) v0) :named IP_51))
(assert (! (or v3 (distinct v4 (= v0 v4 v3 v3 v4)) (is_int 1193124502.0)) :named IP_52))
(assert (! (or (is_int 1193124502.0) (distinct v4 (= v0 v4 v3 v3 v4)) (not v4)) :named IP_53))
(assert (! (or (= v0 v4 v3 v3 v4) (< 397 (div (- i1) 60)) (distinct (- i1) 35)) :named IP_54))
(assert (! (or (distinct (- i1) 35) v3 (not v4)) :named IP_55))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) v0) :named IP_56))
(assert (! (or (= v0 v4 v3 v3 v4) (< 397 (div (- i1) 60)) (distinct (- i1) 35)) :named IP_57))
(assert (! (or (not v4) v0 (< 397 (div (- i1) 60))) :named IP_58))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) (not v4)) :named IP_59))
(assert (! (or (= v0 v4 v3 v3 v4) (distinct (- i1) 35) (is_int 1193124502.0)) :named IP_60))
(assert (! (or (is_int 1193124502.0) v0 (distinct (- i1) 35)) :named IP_61))
(assert (! (or (not v4) (< 397 (div (- i1) 60)) v3) :named IP_62))
(assert (! (or (not v4) (distinct (- i1) 35) v0) :named IP_63))
(assert (! (or (distinct (- i1) 35) (is_int 1193124502.0) v3) :named IP_64))
(assert (! (or (= v0 v4 v3 v3 v4) (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4)) :named IP_65))
(assert (! (or (< 397 (div (- i1) 60)) v3 v3) :named IP_66))
(assert (! (or (not v4) v3 (distinct (- i1) 35)) :named IP_67))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v3 v3) :named IP_68))
(assert (! (or (distinct (- i1) 35) (distinct (- i1) 35) v0) :named IP_69))
(assert (! (or v3 (not v4) (< 397 (div (- i1) 60))) :named IP_70))
(assert (! (or (< 397 (div (- i1) 60)) (is_int 1193124502.0) (< 397 (div (- i1) 60))) :named IP_71))
(assert (! (or (< 397 (div (- i1) 60)) (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_72))
(assert (! (or (is_int 1193124502.0) (distinct v4 (= v0 v4 v3 v3 v4)) (not v4)) :named IP_73))
(assert (! (or v0 v0 (is_int 1193124502.0)) :named IP_74))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_75))
(assert (! (or v3 (not v4) v0) :named IP_76))
(assert (! (or (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4)) (not v4)) :named IP_77))
(assert (! (or v3 (is_int 1193124502.0) (< 397 (div (- i1) 60))) :named IP_78))
(assert (! (or (is_int 1193124502.0) (not v4) v0) :named IP_79))
(assert (! (or (not v4) v3 (not v4)) :named IP_80))
(assert (! (or v3 v0 v0) :named IP_81))
(assert (! (or (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4) (is_int 1193124502.0)) :named IP_82))
(assert (! (or (is_int 1193124502.0) (= v0 v4 v3 v3 v4) (not v4)) :named IP_83))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (is_int 1193124502.0) v0) :named IP_84))
(assert (! (or (not v4) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_85))
(assert (! (or (= v0 v4 v3 v3 v4) v0 (= v0 v4 v3 v3 v4)) :named IP_86))
(assert (! (or v3 (distinct (- i1) 35) (not v4)) :named IP_87))
(assert (! (or (distinct (- i1) 35) v0 (not v4)) :named IP_88))
(assert (! (or (distinct (- i1) 35) (< 397 (div (- i1) 60)) (distinct (- i1) 35)) :named IP_89))
(assert (! (or (distinct (- i1) 35) (< 397 (div (- i1) 60)) (not v4)) :named IP_90))
(assert (! (or (= v0 v4 v3 v3 v4) (is_int 1193124502.0) (distinct (- i1) 35)) :named IP_91))
(assert (! (or (is_int 1193124502.0) (< 397 (div (- i1) 60)) (distinct (- i1) 35)) :named IP_92))
(assert (! (or v0 (distinct (- i1) 35) (is_int 1193124502.0)) :named IP_93))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) (= v0 v4 v3 v3 v4)) :named IP_94))
(assert (! (or (= v0 v4 v3 v3 v4) (distinct (- i1) 35) (= v0 v4 v3 v3 v4)) :named IP_95))
(assert (! (or v3 (not v4) (is_int 1193124502.0)) :named IP_96))
(assert (! (or (is_int 1193124502.0) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_97))
(assert (! (or (= v0 v4 v3 v3 v4) v3 (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_98))
(assert (! (or (distinct (- i1) 35) (distinct v4 (= v0 v4 v3 v3 v4)) v3) :named IP_99))
(assert (! (or v0 v0 (= v0 v4 v3 v3 v4)) :named IP_100))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_101))
(assert (! (or (is_int 1193124502.0) (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_102))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4)) :named IP_103))
(assert (! (or (not v4) (not v4) (= v0 v4 v3 v3 v4)) :named IP_104))
(assert (! (or (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35)) :named IP_105))
(assert (! (or (= v0 v4 v3 v3 v4) (< 397 (div (- i1) 60)) (is_int 1193124502.0)) :named IP_106))
(assert (! (or v0 (is_int 1193124502.0) (distinct (- i1) 35)) :named IP_107))
(assert (! (or (< 397 (div (- i1) 60)) (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_108))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v3 (distinct (- i1) 35)) :named IP_109))
(assert (! (or (= v0 v4 v3 v3 v4) v0 (not v4)) :named IP_110))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4) (is_int 1193124502.0)) :named IP_111))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4) (distinct (- i1) 35)) :named IP_112))
(assert (! (or v0 (not v4) (< 397 (div (- i1) 60))) :named IP_113))
(assert (! (or v3 (distinct (- i1) 35) v0) :named IP_114))
(assert (! (or (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_115))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35)) :named IP_116))
(assert (! (or (< 397 (div (- i1) 60)) (is_int 1193124502.0) (not v4)) :named IP_117))
(assert (! (or v0 (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_118))
(assert (! (or (distinct (- i1) 35) (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_119))
(assert (! (or (= v0 v4 v3 v3 v4) (is_int 1193124502.0) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_120))
(assert (! (or v3 v3 v3) :named IP_121))
(assert (! (or (is_int 1193124502.0) (< 397 (div (- i1) 60)) v3) :named IP_122))
(assert (! (or (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_123))
(assert (! (or (not v4) (= v0 v4 v3 v3 v4) v3) :named IP_124))
(assert (! (or (distinct (- i1) 35) (< 397 (div (- i1) 60)) (< 397 (div (- i1) 60))) :named IP_125))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35) (not v4)) :named IP_126))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v0 v0) :named IP_127))
(assert (! (or (is_int 1193124502.0) (distinct (- i1) 35) (is_int 1193124502.0)) :named IP_128))
(assert (! (or v0 (= v0 v4 v3 v3 v4) (= v0 v4 v3 v3 v4)) :named IP_129))
(assert (! (or v0 (distinct (- i1) 35) v0) :named IP_130))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (= v0 v4 v3 v3 v4) (is_int 1193124502.0)) :named IP_131))
(assert (! (or (not v4) v3 (distinct (- i1) 35)) :named IP_132))
(assert (! (or v0 (< 397 (div (- i1) 60)) (= v0 v4 v3 v3 v4)) :named IP_133))
(assert (! (or (is_int 1193124502.0) (distinct v4 (= v0 v4 v3 v3 v4)) (< 397 (div (- i1) 60))) :named IP_134))
(assert (! (or v0 (distinct v4 (= v0 v4 v3 v3 v4)) (is_int 1193124502.0)) :named IP_135))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4))) :named IP_136))
(assert (! (or v3 (= v0 v4 v3 v3 v4) (< 397 (div (- i1) 60))) :named IP_137))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v3 (distinct (- i1) 35)) :named IP_138))
(assert (! (or v3 (< 397 (div (- i1) 60)) (distinct (- i1) 35)) :named IP_139))
(assert (! (or (< 397 (div (- i1) 60)) (< 397 (div (- i1) 60)) v3) :named IP_140))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (< 397 (div (- i1) 60)) v0) :named IP_141))
(assert (! (or (< 397 (div (- i1) 60)) (not v4) v3) :named IP_142))
(assert (! (or (< 397 (div (- i1) 60)) (distinct v4 (= v0 v4 v3 v3 v4)) (distinct (- i1) 35)) :named IP_143))
(assert (! (or v3 (is_int 1193124502.0) v3) :named IP_144))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) v0 (< 397 (div (- i1) 60))) :named IP_145))
(assert (! (or (distinct v4 (= v0 v4 v3 v3 v4)) (not v4) v3) :named IP_146))
(check-sat-assuming (IP_107 IP_133))
(get-unsat-core)
(exit)