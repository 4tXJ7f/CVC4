; COMMAND-LINE: --full-saturate-quant
; EXPECT: unsat

;; produced by cvc4_16.drv ;;
(set-info :smt-lib-version 2.6)
(set-logic AUFBVFPDTNIRA)
;;; generated by SMT-LIB2 driver
;;; SMT-LIB2 driver: bit-vectors, common part
;;; SMT-LIB2: integer arithmetic
(declare-datatypes ((tuple0 0))
(((Tuple0))))
(declare-sort us_private 0)

(declare-fun private__bool_eq (us_private us_private) Bool)

(declare-const us_null_ext__ us_private)

(declare-sort us_type_of_heap 0)

(declare-datatypes ((us_type_of_heap__ref 0))
(((us_type_of_heap__refqtmk (us_type_of_heap__content us_type_of_heap)))))
(declare-sort us_image 0)

(declare-datatypes ((int__ref 0))
(((int__refqtmk (int__content Int)))))
(declare-datatypes ((bool__ref 0))
(((bool__refqtmk (bool__content Bool)))))
(declare-datatypes ((us_fixed__ref 0))
(((us_fixed__refqtmk (us_fixed__content Int)))))
(declare-datatypes ((real__ref 0))
(((real__refqtmk (real__content Real)))))
(declare-datatypes ((us_private__ref 0))
(((us_private__refqtmk (us_private__content us_private)))))
(define-fun int__ref___projection ((a int__ref)) Int (int__content a))

(define-fun us_fixed__ref___projection ((a us_fixed__ref)) Int (us_fixed__content
                                                               a))

(define-fun bool__ref___projection ((a bool__ref)) Bool (bool__content a))

(define-fun real__ref___projection ((a real__ref)) Real (real__content a))

(define-fun us_private__ref___projection ((a us_private__ref)) us_private 
  (us_private__content a))

(define-fun in_range ((x Int)) Bool (or (= x 0) (= x 1)))

(declare-fun attr__ATTRIBUTE_IMAGE (Bool) us_image)

(declare-fun attr__ATTRIBUTE_VALUE__pre_check (us_image) Bool)

(declare-fun attr__ATTRIBUTE_VALUE (us_image) Bool)

(declare-sort integer 0)

(declare-fun integerqtint (integer) Int)


(declare-fun attr__ATTRIBUTE_IMAGE1 (Int) us_image)

(declare-fun attr__ATTRIBUTE_VALUE__pre_check1 (us_image) Bool)

(declare-fun attr__ATTRIBUTE_VALUE1 (us_image) Int)

(declare-fun user_eq (integer integer) Bool)

(declare-const dummy integer)

(declare-datatypes ((integer__ref 0))
(((integer__refqtmk (integer__content integer)))))
(define-fun integer__ref_integer__content__projection ((a integer__ref)) integer 
  (integer__content a))

(define-fun to_rep ((x integer)) Int (integerqtint x))

(declare-fun of_rep (Int) integer)


(declare-sort positive 0)

(declare-fun positiveqtint (positive) Int)

(declare-datatypes ((positive__ref 0))
(((positive__refqtmk (positive__content positive)))))
(define-fun positive__ref_positive__content__projection ((a positive__ref)) positive 
  (positive__content a))

(declare-datatypes ((us_split_fields 0))
(((us_split_fieldsqtmk
  (rec__test_route__point__x integer)(rec__test_route__point__y integer)))))
(define-fun us_split_fields_rec__test_route__point__x__projection ((a us_split_fields)) integer 
  (rec__test_route__point__x a))

(define-fun us_split_fields_rec__test_route__point__y__projection ((a us_split_fields)) integer 
  (rec__test_route__point__y a))

(declare-datatypes ((us_split_fields__ref 0))
(((us_split_fields__refqtmk (us_split_fields__content us_split_fields)))))
(define-fun us_split_fields__ref___split_fields__content__projection ((a us_split_fields__ref)) us_split_fields 
  (us_split_fields__content a))

(declare-datatypes ((us_rep 0))
(((us_repqtmk (us_split_fields1 us_split_fields)))))
(define-fun us_rep___split_fields__projection ((a us_rep)) us_split_fields 
  (us_split_fields1 a))


(declare-const value__size Int)

(declare-const object__size Int)

(declare-const alignment Int)

(declare-const test_route__point__x__first__bit Int)

(declare-const test_route__point__x__last__bit Int)

(declare-const test_route__point__x__position Int)

(declare-const test_route__point__y__first__bit Int)

(declare-const test_route__point__y__last__bit Int)

(declare-const test_route__point__y__position Int)

(declare-fun user_eq2 (us_rep us_rep) Bool)

(declare-const dummy2 us_rep)

(declare-datatypes ((point__ref 0))
(((point__refqtmk (point__content us_rep)))))
(define-fun point__ref_point__content__projection ((a point__ref)) us_rep 
  (point__content a))

(declare-datatypes ((us_rep1 0))
(((us_repqtmk1
  (rec__test_route__point_acc__is_null_pointer Bool)(rec__test_route__point_acc__pointer_address Int)(rec__test_route__point_acc__pointer_value us_rep)))))
(define-fun us_rep_rec__test_route__point_acc__is_null_pointer__projection ((a us_rep1)) Bool 
  (rec__test_route__point_acc__is_null_pointer a))

(define-fun us_rep_rec__test_route__point_acc__pointer_address__projection ((a us_rep1)) Int 
  (rec__test_route__point_acc__pointer_address a))

(define-fun us_rep_rec__test_route__point_acc__pointer_value__projection ((a us_rep1)) us_rep 
  (rec__test_route__point_acc__pointer_value a))

(declare-datatypes ((us_rep__ref 0))
(((us_rep__refqtmk (us_rep__content us_rep1)))))
(define-fun us_rep__ref___rep__content__projection ((a us_rep__ref)) us_rep1 
  (us_rep__content a))

(define-fun rec__test_route__point_acc__pointer_value__pred ((a us_rep1)) Bool 
  (not (= (rec__test_route__point_acc__is_null_pointer a) true)))

(declare-const us_null_pointer us_rep1)

;; __null_pointer__def_axiom
  (assert
  (= (rec__test_route__point_acc__is_null_pointer us_null_pointer) true))

(declare-const dummy3 us_rep1)

(declare-datatypes ((t1b__ref 0))
(((t1b__refqtmk (t1b__content us_rep1)))))
(define-fun t1b__ref_t1b__content__projection ((a t1b__ref)) us_rep1 
  (t1b__content a))

(declare-sort us_main_type 0)

(declare-datatypes ((us_rep2 0))
(((us_repqtmk2
  (rec__test_route__route_acc__is_null_pointer Bool)(rec__test_route__route_acc__pointer_address Int)(rec__test_route__route_acc__pointer_value_abstr us_main_type)))))
(define-fun us_rep_rec__test_route__route_acc__is_null_pointer__projection ((a us_rep2)) Bool 
  (rec__test_route__route_acc__is_null_pointer a))

(define-fun us_rep_rec__test_route__route_acc__pointer_address__projection ((a us_rep2)) Int 
  (rec__test_route__route_acc__pointer_address a))

(define-fun us_rep_rec__test_route__route_acc__pointer_value_abstr__projection ((a us_rep2)) us_main_type 
  (rec__test_route__route_acc__pointer_value_abstr a))

(declare-datatypes ((us_rep__ref1 0))
(((us_rep__refqtmk1 (us_rep__content1 us_rep2)))))
(define-fun us_rep__ref___rep__content__2__projection ((a us_rep__ref1)) us_rep2 
  (us_rep__content1 a))


(declare-const dummy4 us_rep2)

(declare-datatypes ((t4b__ref 0))
(((t4b__refqtmk (t4b__content us_rep2)))))
(define-fun t4b__ref_t4b__content__projection ((a t4b__ref)) us_rep2 
  (t4b__content a))

(declare-fun length (us_rep2) Int)

(declare-fun length__function_guard (Int us_rep2) Bool)

(define-fun min ((x Int) (y Int)) Int (ite (<= x y) x y))

(define-fun max ((x Int) (y Int)) Int (ite (<= x y) y x))

(declare-sort natural 0)

(declare-fun naturalqtint (natural) Int)

(define-fun in_range3 ((x Int)) Bool (and (<= 0 x) (<= x 2147483647)))

(declare-fun attr__ATTRIBUTE_IMAGE3 (Int) us_image)

(declare-fun attr__ATTRIBUTE_VALUE__pre_check3 (us_image) Bool)

(declare-fun attr__ATTRIBUTE_VALUE3 (us_image) Int)

(declare-fun user_eq3 (natural natural) Bool)

(declare-const dummy5 natural)

(declare-datatypes ((natural__ref 0))
(((natural__refqtmk (natural__content natural)))))
(define-fun natural__ref_natural__content__projection ((a natural__ref)) natural 
  (natural__content a))

(declare-const dummy6 us_rep1)

(declare-datatypes ((point_acc__ref 0))
(((point_acc__refqtmk (point_acc__content us_rep1)))))
(define-fun point_acc__ref_point_acc__content__projection ((a point_acc__ref)) us_rep1 
  (point_acc__content a))

(declare-const dummy7 us_rep2)

(declare-datatypes ((route_acc__ref 0))
(((route_acc__refqtmk (route_acc__content us_rep2)))))
(define-fun route_acc__ref_route_acc__content__projection ((a route_acc__ref)) us_rep2 
  (route_acc__content a))

(declare-datatypes ((us_split_fields2 0))
(((us_split_fieldsqtmk1
  (rec__test_route__route__current us_rep1)(rec__test_route__route__rest us_rep2)))))
(define-fun us_split_fields_rec__test_route__route__current__projection ((a us_split_fields2)) us_rep1 
  (rec__test_route__route__current a))

(define-fun us_split_fields_rec__test_route__route__rest__projection ((a us_split_fields2)) us_rep2 
  (rec__test_route__route__rest a))

(declare-datatypes ((us_split_fields__ref1 0))
(((us_split_fields__refqtmk1 (us_split_fields__content1 us_split_fields2)))))
(define-fun us_split_fields__ref___split_fields__content__2__projection ((a us_split_fields__ref1)) us_split_fields2 
  (us_split_fields__content1 a))

(declare-datatypes ((us_rep3 0))
(((us_repqtmk3 (us_split_fields3 us_split_fields2)))))
(define-fun us_rep___split_fields__2__projection ((a us_rep3)) us_split_fields2 
  (us_split_fields3 a))

(declare-const value__size1 Int)

(declare-const object__size1 Int)

(declare-const alignment1 Int)

(declare-const test_route__route__current__first__bit Int)

(declare-const test_route__route__current__last__bit Int)

(declare-const test_route__route__current__position Int)


(declare-const test_route__route__rest__first__bit Int)

(declare-const test_route__route__rest__last__bit Int)

(declare-const test_route__route__rest__position Int)


(declare-fun user_eq4 (us_rep3 us_rep3) Bool)

(declare-const dummy8 us_rep3)

(declare-datatypes ((route__ref 0))
(((route__refqtmk (route__content us_rep3)))))
(define-fun route__ref_route__content__projection ((a route__ref)) us_rep3 
  (route__content a))

(declare-fun us_open (us_main_type) us_rep3)

(declare-fun us_close (us_rep3) us_main_type)

(define-fun rec__test_route__route_acc__pointer_value ((a us_rep2)) us_rep3 
  (us_open (rec__test_route__route_acc__pointer_value_abstr a)))

(define-fun rec__test_route__route_acc__pointer_value__pred ((a us_rep2)) Bool 
  (not (= (rec__test_route__route_acc__is_null_pointer a) true)))

(declare-const us_null_pointer1 us_rep2)


(declare-fun temp___dynamic_invariant_202 (us_rep2 Bool Bool Bool Bool) Bool)



(declare-const dummy9 us_rep2)

(declare-datatypes ((t5b__ref 0))
(((t5b__refqtmk (t5b__content us_rep2)))))
(define-fun t5b__ref_t5b__content__projection ((a t5b__ref)) us_rep2 
  (t5b__content a))

(declare-fun nth_x (us_rep2 Int) Int)

(declare-fun nth_x__function_guard (Int us_rep2 Int) Bool)

(declare-datatypes ((map__ref 0))
(((map__refqtmk (map__content (Array Int integer))))))
(declare-fun slide ((Array Int integer) Int Int) (Array Int integer))


(declare-sort t 0)

(declare-fun first (t) integer)

(declare-fun last (t) integer)

(declare-fun mk (Int Int) t)

(declare-datatypes ((us_t 0))
(((us_tqtmk (elts (Array Int integer))(rt t)))))

(declare-const value__size2 Int)

(declare-const object__size2 Int)

(declare-const component__size Int)

(declare-const alignment2 Int)


(declare-fun user_eq5 (us_t us_t) Bool)

(declare-const dummy10 us_t)

(declare-datatypes ((int_array__ref 0))
(((int_array__refqtmk (int_array__content us_t)))))
(define-fun int_array__ref_int_array__content__projection ((a int_array__ref)) us_t 
  (int_array__content a))

(declare-const dummy11 us_rep2)

(declare-datatypes ((t9b__ref 0))
(((t9b__refqtmk (t9b__content us_rep2)))))
(define-fun t9b__ref_t9b__content__projection ((a t9b__ref)) us_rep2 
  (t9b__content a))


(declare-const dummy12 us_rep2)

(declare-datatypes ((t21b__ref 0))
(((t21b__refqtmk (t21b__content us_rep2)))))
(define-fun t21b__ref_t21b__content__projection ((a t21b__ref)) us_rep2 
  (t21b__content a))

(declare-const dummy13 us_rep1)

(declare-datatypes ((t22b__ref 0))
(((t22b__refqtmk (t22b__content us_rep1)))))
(define-fun t22b__ref_t22b__content__projection ((a t22b__ref)) us_rep1 
  (t22b__content a))

(declare-fun nth_point (us_rep2 Int) us_rep1)

(declare-fun nth_point__function_guard (us_rep1 us_rep2 Int) Bool)

(declare-sort us_pledge_ty 0)

(declare-datatypes ((us_pledge_ty__ref 0))
(((us_pledge_ty__refqtmk (us_pledge_ty__content us_pledge_ty)))))
(declare-fun us_pledge_get (us_pledge_ty us_rep2 us_rep1) Bool)

(declare-fun test_route__nth_point__pledge (us_rep2 Int) us_pledge_ty)

(declare-const r__pointer_address Int)

(declare-const r__is_null_pointer Bool)

(declare-const attr__ATTRIBUTE_ADDRESS Int)

(declare-const n Int)

(declare-const attr__ATTRIBUTE_ADDRESS1 Int)

(declare-const s Int)

(declare-const attr__ATTRIBUTE_ADDRESS2 Int)

(declare-const l Int)

(declare-const attr__ATTRIBUTE_ADDRESS3 Int)

(declare-const dummy14 us_rep2)

(declare-datatypes ((t29b__ref 0))
(((t29b__refqtmk (t29b__content us_rep2)))))
(define-fun t29b__ref_t29b__content__projection ((a t29b__ref)) us_rep2 
  (t29b__content a))

(declare-const dummy15 us_rep1)

(declare-datatypes ((t34b__ref 0))
(((t34b__refqtmk (t34b__content us_rep1)))))
(define-fun t34b__ref_t34b__content__projection ((a t34b__ref)) us_rep1 
  (t34b__content a))

(declare-const attr__ATTRIBUTE_ADDRESS4 Int)

(declare-sort us_pledge_ty1 0)

(declare-datatypes ((us_pledge_ty__ref1 0))
(((us_pledge_ty__refqtmk1 (us_pledge_ty__content1 us_pledge_ty1)))))
(declare-fun us_pledge_get1 (us_pledge_ty1 us_rep2 us_rep1) Bool)


(declare-const r__pointer_value us_split_fields2)


(define-fun o () us_rep2 (us_repqtmk2 r__is_null_pointer r__pointer_address
                         (us_close (us_repqtmk3 r__pointer_value))))

(define-fun test_route__shift_nth_x__l__assume () Int (length o))


(define-fun o1 () Int n)

(define-fun o2 () us_rep2 (us_repqtmk2 r__is_null_pointer r__pointer_address
                          (us_close (us_repqtmk3 r__pointer_value))))

(define-fun test_route__shift_nth_x__p__assume () us_rep1 (nth_point o2 o1))



(declare-const usf us_pledge_ty1)


(declare-const test_route__shift_nth_x__p__pledge us_pledge_ty1)


(declare-const p__pointer_value us_split_fields)


(declare-const p__pointer_address Int)


(declare-const p__is_null_pointer Bool)


(declare-const temp___borrowed_250 us_rep2)

(declare-const temp___brower_249 us_rep1)

;; H
  (assert

  (forall ((temp___borrowed_236 us_rep2))
  (forall ((temp___brower_235 us_rep1))
   (=>
     (and
     (and
     (= (us_pledge_get (test_route__nth_point__pledge o2 n) temp___borrowed_236
        temp___brower_235) true)
     (not
     (= (rec__test_route__point_acc__is_null_pointer temp___brower_235) true)))
     (and
     (= r__is_null_pointer (rec__test_route__route_acc__is_null_pointer
                           temp___borrowed_236))
     (= (rec__test_route__point_acc__is_null_pointer
        test_route__shift_nth_x__p__assume) (rec__test_route__point_acc__is_null_pointer
                                            temp___brower_235))))
     (= (length temp___borrowed_236) (length o2))))))
							     
(assert (not (=>
     (and
     (and
     (= (us_pledge_get (test_route__nth_point__pledge o2 n) temp___borrowed_250
        temp___brower_249) true)
     (not
     (= (rec__test_route__point_acc__is_null_pointer temp___brower_249) true)))
     (and
     (= r__is_null_pointer (rec__test_route__route_acc__is_null_pointer
                           temp___borrowed_250))
     (= (rec__test_route__point_acc__is_null_pointer
        test_route__shift_nth_x__p__assume) (rec__test_route__point_acc__is_null_pointer
                                            temp___brower_249))))
     (= (length temp___borrowed_250) (length o2)))))

(check-sat)
