/*********************                                                        */
/*! \file datatypes_sygus.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds, Paul Meng, Tim King
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2017 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Sygus utilities for theory of datatypes
 **
 ** Theory of datatypes.
 **/

#include "cvc4_private.h"

#ifndef __CVC4__THEORY__DATATYPES__DATATYPES_SYGUS_NEW_H
#define __CVC4__THEORY__DATATYPES__DATATYPES_SYGUS_NEW_H

#include <iostream>
#include <map>

#include "context/cdhashmap.h"
#include "context/cdhashset.h"
#include "context/cdlist.h"
#include "context/cdo.h"
#include "context/context.h"
#include "expr/datatype.h"
#include "expr/node.h"
#include "theory/quantifiers/sygus/ce_guided_conjecture.h"
#include "theory/quantifiers/sygus/sygus_explain.h"
#include "theory/quantifiers/sygus_sampler.h"
#include "theory/quantifiers/term_database.h"

namespace CVC4 {
namespace theory {
namespace datatypes {

class TheoryDatatypes;

/**
 * This is the sygus extension of the decision procedure for quantifier-free
 * inductive datatypes. At a high level, this class takes as input a
 * set of asserted testers is-C1( x ), is-C2( x.1 ), is-C3( x.2 ), and
 * generates lemmas that restrict the models of x, if x is a "sygus enumerator"
 * (see TermDbSygus::registerEnumerator).
 *
 * Some of these techniques are described in these papers:
 * "Refutation-Based Synthesis in SMT", Reynolds et al 2017.
 * "Sygus Techniques in the Core of an SMT Solver", Reynolds et al 2017.
 */
class SygusSymBreakNew
{
  typedef context::CDHashMap< Node, int, NodeHashFunction > IntMap;
  typedef context::CDHashMap< Node, Node, NodeHashFunction > NodeMap;
  typedef context::CDHashMap< Node, bool, NodeHashFunction > BoolMap;
  typedef context::CDHashSet<Node, NodeHashFunction> NodeSet;

 public:
  SygusSymBreakNew(TheoryDatatypes* td,
                   quantifiers::TermDbSygus* tds,
                   context::Context* c);
  ~SygusSymBreakNew();
  /**
   * Notify this class that tester for constructor tindex has been asserted for
   * n. Exp is the literal corresponding to this tester. This method may add
   * lemmas to the vector lemmas, for details see assertTesterInternal below.
   * These lemmas are sent out on the output channel of datatypes by the caller.
   */
  void assertTester(int tindex, TNode n, Node exp, std::vector<Node>& lemmas);
  /**
   * Notify this class that literal n has been asserted with the given
   * polarity. This method may add lemmas to the vector lemmas, for instance
   * based on inferring consequences of (not) n. One example is if n is
   * (DT_SIZE_BOUND x n), we add the lemma:
   *   (DT_SIZE_BOUND x n) <=> ((DT_SIZE x) <= n )
   */
  void assertFact(Node n, bool polarity, std::vector<Node>& lemmas);
  /** pre-register term n
   *
   * This is called when n is pre-registered with the theory of datatypes.
   * If n is a sygus enumerator, then we may add lemmas to the vector lemmas
   * that are used to enforce fairness regarding the size of n.
   */
  void preRegisterTerm(TNode n, std::vector<Node>& lemmas);
  /** check
   *
   * This is called at last call effort, when the current model assignment is
   * satisfiable according to the quantifier-free decision procedures and a
   * model is built. This method may add lemmas to the vector lemmas based
   * on dynamic symmetry breaking techniques, based on the model values of
   * all preregistered enumerators.
   */
  void check(std::vector<Node>& lemmas);
  /** get next decision request
   *
   * This function has the same interface as Theory::getNextDecisionRequest.
   *
   * The decisions returned by this method are of one of two forms:
   * (1) Positive decisions on the active guards G of enumerators e registered
   * to this class. These assert "there are more values to enumerate for e".
   * (2) Positive bounds (DT_SYGUS_BOUND m n) for "measure terms" m (see below),
   * where n is a non-negative integer. This asserts "the measure of terms
   * we are enumerating for enumerators whose measure term m is at most n",
   * where measure is commonly term size, but can also be height.
   *
   * We prioritize decisions of form (1) before (2). For both decisions,
   * we set the priority argument to "1", indicating that the decision is
   * critical for solution completeness.
   */
  Node getNextDecisionRequest(unsigned& priority, std::vector<Node>& lemmas);

 private:
  /** Pointer to the datatype theory that owns this class. */
  TheoryDatatypes* d_td;
  /** Pointer to the sygus term database */
  quantifiers::TermDbSygus* d_tds;
  /**
   * Map from terms to the index of the tester that is asserted for them in
   * the current SAT context. In other words, if d_testers[n] = 2, then the
   * tester is-C_2(n) is asserted in this SAT context.
   */
  IntMap d_testers;
  /**
   * Map from terms to the tester asserted for them. In the example above,
   * d_testers[n] = is-C_2(n).
   */
  NodeMap d_testers_exp;
  /**
   * The set of (selector chain) terms that are active in the current SAT
   * context. A selector chain term S_n( ... S_1( x )... ) is active if either:
   * (1) n=0 and x is a sygus enumerator,
   *   or:
   * (2.1) S_{n-1}( ... S_1( x )) is active,
   * (2.2) is-C( S_{n-1}( ... S_1( x )) ) is asserted in this SAT context, and
   * (2.3) S_n is a selector for constructor C.
   */
  NodeSet d_active_terms;
  /**
   * Map from enumerators to a lower bound on their size in the current SAT
   * context.
   */
  IntMap d_currTermSize;
  /** zero */
  Node d_zero;
  /** true */
  Node d_true;
  /**
   * Map from terms (selector chains) to their anchors. The anchor of a
   * selector chain S1( ... Sn( x ) ... ) is x.
   */
  std::unordered_map<Node, Node, NodeHashFunction> d_term_to_anchor;
  /**
   * Map from anchors to the conjecture they are associated with.
   */
  std::map<Node, quantifiers::CegConjecture*> d_anchor_to_conj;
  /**
   * Map from terms (selector chains) to their depth. The depth of a selector
   * chain S1( ... Sn( x ) ... ) is:
   *   weight( S1 ) + ... + weight( Sn ),
   * where weight is the selector weight of Si
   * (see SygusTermDatabase::getSelectorWeight).
   */
  std::unordered_map<Node, unsigned, NodeHashFunction> d_term_to_depth;
  /**
   * Map from terms (selector chains) to whether they are the topmost term
   * of their type. For example, if:
   *   S1 : T1 -> T2
   *   S2 : T2 -> T2
   *   S3 : T2 -> T1
   *   S4 : T1 -> T3
   * Then, x, S1( x ), and S4( S3( S2( S1( x ) ) ) ) are top-level terms,
   * whereas S2( S1( x ) ) and S3( S2( S1( x ) ) ) are not.
   */
  std::unordered_map<Node, bool, NodeHashFunction> d_is_top_level;
  /**
   * Returns true if the selector chain n is top-level based on the above
   * definition, when tn is the type of n.
   */
  bool computeTopLevel( TypeNode tn, Node n );
private:
 /** This caches all information regarding symmetry breaking for an anchor. */
 class SearchCache
 {
  public:
    SearchCache(){}
    /**
     * A cache of all search terms for (types, sizes). See registerSearchTerm
     * for definition of search terms.
     */
    std::map< TypeNode, std::map< unsigned, std::vector< Node > > > d_search_terms;
    /** A cache of all symmetry breaking lemma templates for (types, sizes). */
    std::map< TypeNode, std::map< unsigned, std::vector< Node > > > d_sb_lemmas;
    /** search value
     *
     * For each sygus type, a map from a builtin term to a sygus term for that
     * type that we encountered during the search whose analog rewrites to that
     * term. The range of this map can be updated if we later encounter a sygus
     * term that also rewrites to the builtin value but has a smaller term size.
     */
    std::map<TypeNode, std::unordered_map<Node, Node, NodeHashFunction>>
        d_search_val;
    /** the size of terms in the range of d_search val. */
    std::map<TypeNode, std::unordered_map<Node, unsigned, NodeHashFunction>>
        d_search_val_sz;
    /**
     * For each type tn, this stores all builtin constants in the domain of
     * d_search_val[tn].
     */
    std::map<TypeNode, std::unordered_set<Node, NodeHashFunction>>
        d_search_val_consts;
    /**
     * The (finite) cardinality of each type if it exists, or -1 if the
     * cardinality of the type is large or infinite).
     */
    std::map<TypeNode, int> d_search_val_const_cardinality;
    /** explanation for why any search value is a constant */
    std::map<TypeNode, Node> d_isconst_rec_pred;
    /** For each term, whether this cache has processed that term */
    std::unordered_set<Node, NodeHashFunction> d_search_val_proc;
  };
  /** An instance of the above cache, for each anchor */
  std::map< Node, SearchCache > d_cache;
  /** a sygus sampler object for each (anchor, sygus type) pair
   *
   * This is used for the sygusRewVerify() option to verify the correctness of
   * the rewriter.
   */
  std::map<Node, std::map<TypeNode, quantifiers::SygusSampler>> d_sampler;
  /** Assert tester internal
   *
   * This function is called when the tester with index tindex is asserted for
   * n, exp is the tester predicate. For example, for grammar:
   *   A -> A+A | x | 1 | 0
   * when is_+( d ) is asserted,
   * assertTesterInternal(0, s( d ), is_+( s( d ) ),...) is called. This
   * function may add lemmas to lemmas, which are sent out on the output
   * channel of datatypes by the caller.
   *
   * These lemmas are of various forms, including:
   * (1) dynamic symmetry breaking clauses for subterms of n (those added to
   * lemmas on calls to addSymBreakLemmasFor, see function below),
   * (2) static symmetry breaking clauses for subterms of n (those added to
   * lemmas on getSimpleSymBreakPred, see function below),
   * (3) conjecture-specific symmetry breaking lemmas, see
   * CegConjecture::getSymmetryBreakingPredicate,
   * (4) fairness conflicts if sygusFair() is SYGUS_FAIR_DIRECT, e.g.:
   *    size( d ) <= 1 V ~is-C1( d ) V ~is-C2( d.1 )
   * where C1 and C2 are non-nullary constructors.
   */
  void assertTesterInternal( int tindex, TNode n, Node exp, std::vector< Node >& lemmas );
  /**
   * This function is called when term n is registered to the theory of
   * datatypes. It makes the appropriate call to registerSearchTerm below,
   * if applicable.
   */
  void registerTerm(Node n, std::vector<Node>& lemmas);

  //------------------------dynamic symmetry breaking
  /** Register search term
   *
   * This function is called when selector chain n of the form
   * S_1( ... S_m( x ) ... ) is registered to the theory of datatypes, where
   * tn is the type of n, d indicates the depth of n (the sum of weights of the
   * selectors S_1...S_m), and topLevel is whether n is a top-level term
   * (see d_is_top_level). We refer to n as a "search term".
   *
   * The purpose of this function is to notify this class that symmetry breaking
   * lemmas should be instantiated for n. Any symmetry breaking lemmas that
   * are active for n (see description of addSymBreakLemmasFor) are added to
   * lemmas in this call.
   */
  void registerSearchTerm( TypeNode tn, unsigned d, Node n, bool topLevel, std::vector< Node >& lemmas );
  /** Register search value
   *
   * This function is called when a selector chain n has been assigned a model
   * value nv. This function calls itself recursively so that extensions of the
   * selector chain n are registered with all the subterms of nv. For example,
   * if we call this function with:
   *   n = x, nv = +( 1(), x() )
   * we make recursive calls with:
   *   n = x.1, nv = 1() and n = x.2, nv = x()
   *
   * a : the anchor of n,
   * d : the depth of n.
   *
   * This function determines if the value nv is equivalent via rewriting to
   * any previously registered search values for anchor a. If so, we construct
   * a symmetry breaking lemma template and register it in d_cache[a]. For
   * example, for grammar:
   *   A -> A+A | x | 1 | 0
   * Registering search value d -> x followed by d -> +( x, 0 ) results in the
   * construction of the symmetry breaking lemma template:
   *   ~is_+( z ) V ~is_x( z.1 ) V ~is_0( z.2 )
   * which is stored in d_cache[a].d_sb_lemmas. This lemma is instantiated with
   * z -> t for all terms t of appropriate depth, including d.
   * This function strengthens blocking clauses using generalization techniques
   * described in Reynolds et al SYNT 2017.
   *
   * The return value of this function is an abstraction of model assignment
   * of nv to n, or null if we wish to exclude the model assignment nv to n.
   * The return value of this method is different from nv itself, e.g. if it
   * contains occurrences of the "any constant" constructor. For example, if
   * nv is C_+( C_x(), C_{any_constant}( 5 ) ), then the return value of this
   * function will either be null, or C_+( C_x(), C_{any_constant}( n.1.0 ) ),
   * where n.1.0 is the appropriate selector chain applied to n. We build this
   * abstraction since the semantics of C_{any_constant} is "any constant" and
   * not "some constant". Thus, we should consider the subterm
   * C_{any_constant}( 5 ) above to be an unconstrained variable (as represented
   * by a selector chain), instead of the concrete value 5.
   */
  Node registerSearchValue(
      Node a, Node n, Node nv, unsigned d, std::vector<Node>& lemmas);
  /** Register symmetry breaking lemma
   *
   * This function adds the symmetry breaking lemma template lem for terms of
   * type tn with anchor a. This is added to d_cache[a].d_sb_lemmas. Notice that
   * we use lem as a template with free variable x, e.g. our template is:
   *   (lambda ((x tn)) lem)
   * where x = getFreeVar( tn ). For all search terms t of the appropriate
   * depth,
   * we add the lemma lem{ x -> t } to lemmas.
   *
   * The argument sz indicates the size of terms that the lemma applies to, e.g.
   *   ~is_+( z ) has size 1
   *   ~is_+( z ) V ~is_x( z.1 ) V ~is_0( z.2 ) has size 1
   *   ~is_+( z ) V ~is_+( z.1 ) has size 2
   * This is equivalent to sum of weights of constructors corresponding to each
   * tester, e.g. above + has weight 1, and x and 0 have weight 0.
   */
  void registerSymBreakLemma(
      TypeNode tn, Node lem, unsigned sz, Node a, std::vector<Node>& lemmas);
  /** Register symmetry breaking lemma for value
   *
   * This function adds a symmetry breaking lemma template for selector chains
   * with anchor a, that effectively states that val should never be a subterm
   * of any value for a.
   *
   * et : an "invariance test" (see sygus/sygus_invariance.h) which states a
   * criterion that val meets, which is the reason for its exclusion. This is
   * used for generalizing the symmetry breaking lemma template.
   * valr : if non-null, this states a value that should *not* be excluded by
   * the symmetry breaking lemma template, which is a restriction to the above
   * generalization.
   *
   * This function may add instances of the symmetry breaking template for
   * existing search terms, which are added to lemmas.
   */
  void registerSymBreakLemmaForValue(Node a,
                                     Node val,
                                     quantifiers::SygusInvarianceTest& et,
                                     Node valr,
                                     std::map<TypeNode, int>& var_count,
                                     std::vector<Node>& lemmas);
  /** Add symmetry breaking lemmas for term
   *
   * Adds all active symmetry breaking lemmas for selector chain t to lemmas. A
   * symmetry breaking lemma L is active for t based on three factors:
   * (1) the current search size sz(a) for its anchor a,
   * (2) the depth d of term t (see d_term_to_depth),
   * (3) the size sz(L) of the symmetry breaking lemma L.
   * In particular, L is active if sz(L) <= sz(a) - d. In other words, a
   * symmetry breaking lemma is active if it is intended to block terms of
   * size sz(L), and the maximum size that t can take in the current search,
   * sz(a)-d, is greater than or equal to this value.
   *
   * tn : the type of term t,
   * a : the anchor of term t,
   * d : the depth of term t.
   */
  void addSymBreakLemmasFor(
      TypeNode tn, Node t, unsigned d, Node a, std::vector<Node>& lemmas);
  /** calls the above function where a is the anchor t */
  void addSymBreakLemmasFor( TypeNode tn, Node t, unsigned d, std::vector< Node >& lemmas );
  /** get explanation for constant evaluation
   *
   * If this method returns true, then exp implies that the builtin analog of x
   * rewrites to a constant.
   */
  bool getExplanationForEvalConstant(Node x, Node n, std::vector<Node>& exp);
  //------------------------end dynamic symmetry breaking

  /** Get relevancy condition
   *
   * This returns (the negation of) a predicate that holds in the contexts in
   * which the selector chain n is specified. For example, the negation of the
   * relevancy condition for sel_{C2,1}( sel_{C1,1}( d ) ) is
   *    ~( is-C1( d ) ^ is-C2( sel_{C1,1}( d ) ) )
   * If shared selectors are enabled, this is a conjunction of disjunctions,
   * since shared selectors may apply to multiple constructors.
   */
  Node getRelevancyCondition( Node n );
  /** Cache of the above function */
  std::map<Node, Node> d_rlv_cond;

  //------------------------static symmetry breaking
  /** Get simple symmetry breakind predicate
   *
   * This function returns the "static" symmetry breaking lemma template for
   * terms with type tn and constructor index tindex, for the given depth. This
   * includes inferences about size with depth=0. Given grammar:
   *   A -> ite( B, A, A ) | A+A | x | 1 | 0
   *   B -> A = A
   * Examples of static symmetry breaking lemma templates are:
   *   for +, depth 0: size(z)=size(z.1)+size(z.2)+1
   *   for +, depth 1: ~is-0( z.1 ) ^ ~is-0( z.2 ) ^ F
   *     where F ensures the constructor of z.1 is less than that of z.2 based
   *     on some ordering.
   *   for ite, depth 1: z.2 != z.3
   * These templates can be thought of as "hard-coded" cases of dynamic symmetry
   * breaking lemma templates. Notice that the above lemma templates are in
   * terms of getFreeVar( tn ), hence only one is created per
   * (constructor, depth). A static symmetry break lemma template F[z] for
   * constructor C are included in lemmas of the form:
   *   is-C( t ) => F[t]
   * where t is a search term, see registerSearchTerm for definition of search
   * term.
   *
   * usingSymCons is whether we are using symbolic constructors for subterms in
   * the type tn. This may affect the form of the predicate we construct.
   */
  Node getSimpleSymBreakPred(TypeNode tn,
                             int tindex,
                             unsigned depth,
                             bool usingSymCons);
  /** Cache of the above function */
  std::map<TypeNode, std::map<int, std::map<bool, std::map<unsigned, Node>>>>
      d_simple_sb_pred;
  /**
   * For each search term, this stores the maximum depth for which we have added
   * a static symmetry breaking lemma.
   *
   * This should be user context-dependent if sygus is updated to work in
   * incremental mode.
   */
  std::unordered_map<Node, unsigned, NodeHashFunction> d_simple_proc;
  //------------------------end static symmetry breaking

  /** Get the canonical free variable for type tn */
  TNode getFreeVar( TypeNode tn );
  /** get term order predicate
   *
   * Assuming that n1 and n2 are children of a commutative operator, this
   * returns a symmetry breaking predicate that can be instantiated for n1 and
   * n2 while preserving satisfiability. By default, this is the predicate
   *   ( DT_SIZE n1 ) >= ( DT_SIZE n2 )
   */
  Node getTermOrderPredicate( Node n1, Node n2 );
  /** get is-constant predicate
   *
   * Given a term n of sygus datatype type, this returns a predicate that holds
   * iff the builtin version of n is a (possibly symbolic) constant. For
   * example, for:
   *   A -> 0 | 1 | x | A+A
   * this returns the predicate is-0( n ) V is-1( n ).
   */
  Node getIsConstantPredicate(TNode n);
  /** the is-constant predicate, per sygus datatype type
   *
   * For each sygus datatype tn, this predicate states that x is a sygus
   * datatype value corresponding to a constant, where x is getFreeVar( tn ).
   */
  std::map<TypeNode, Node> d_isconst_pred;

 private:
  /**
   * Map from registered variables to whether they are a sygus enumerator.
   *
   * This should be user context-dependent if sygus is updated to work in
   * incremental mode.
   */
  std::map<Node, bool> d_register_st;
  //----------------------search size information
  /**
   * Checks whether e is a sygus enumerator, that is, a term for which this
   * class will track size for.
   *
   * We associate each sygus enumerator e with a "measure term", which is used
   * for bounding the size of terms for the models of e. The measure term for a
   * sygus enumerator may be e itself (if e has an active guard), or an
   * arbitrary sygus variable otherwise. A measure term m is one for which our
   * decision strategy decides on literals of the form (DT_SYGUS_BOUND m n).
   *
   * After determining the measure term m for e, if applicable, we initialize
   * SearchSizeInfo for m below. This may result in lemmas
   */
  void registerSizeTerm(Node e, std::vector<Node>& lemmas);
  /** information for each measure term allocated by this class */
  class SearchSizeInfo
  {
   public:
    SearchSizeInfo( Node t, context::Context* c ) : d_this( t ), d_curr_search_size(0), d_curr_lit( c, 0 ) {}
    /** the measure term */
    Node d_this;
    /**
     * For each size n, an explanation for why this measure term has size at
     * most n. This is typically the literal (DT_SYGUS_BOUND m n), which
     * we call the (n^th) "fairness literal" for m.
     */
    std::map< unsigned, Node > d_search_size_exp;
    /**
     * For each size, whether we have called SygusSymBreakNew::notifySearchSize.
     */
    std::map< unsigned, bool > d_search_size;
    /**
     * The current search size. This corresponds to the number of times
     * incrementCurrentSearchSize has been called for this measure term.
     */
    unsigned d_curr_search_size;
    /** the list of all enumerators whose measure term is this */
    std::vector< Node > d_anchors;
    /** get or make the measure value
     *
     * The measure value is an integer variable v that is a (symbolic) integer
     * value that is constrained to be less than or equal to the current search
     * size. For example, if we are using the fairness strategy
     * SYGUS_FAIR_DT_SIZE (see options/datatype_options.h), then we constrain:
     *   (DT_SYGUS_BOUND m n) <=> (v <= n)
     * for all asserted fairness literals. Then, if we are enforcing fairness
     * based on the maximum size, we assert:
     *   (DT_SIZE e) <= v
     * for all enumerators e.
     */
    Node getOrMkMeasureValue(std::vector<Node>& lemmas);
    /** get or make the active measure value
     *
     * The active measure value av is an integer variable that corresponds to
     * the (symbolic) value of the sum of enumerators that are yet to be
     * registered. This is to enforce the "sum of measures" strategy. For
     * example, if we are using the fairness strategy SYGUS_FAIR_DT_SIZE,
     * then initially av is equal to the measure value v, and the constraints
     *   (DT_SYGUS_BOUND m n) <=> (v <= n)
     * are added as before. When an enumerator e is registered, we add the
     * lemma:
     *   av = (DT_SIZE e) + av'
     * and update the active measure value to av'. This ensures that the sum
     * of sizes of active enumerators is at most n.
     *
     * If the flag mkNew is set to true, then we return a fresh variable and
     * update the active measure value.
     */
    Node getOrMkActiveMeasureValue(std::vector<Node>& lemmas,
                                   bool mkNew = false);
    /**
     * The current search size literal for this measure term. This corresponds
     * to the minimial n such that (DT_SYGUS_BOUND d_this n) is asserted in
     * this SAT context.
     */
    context::CDO< unsigned > d_curr_lit;
    /**
     * Map from integers n to the fairness literal, for each n such that this
     * literal has been allocated (by getFairnessLiteral below).
     */
    std::map< unsigned, Node > d_lits;
    /**
     * Returns the s^th fairness literal for this measure term. This adds a
     * split on this literal to lemmas.
     */
    Node getFairnessLiteral( unsigned s, TheoryDatatypes * d, std::vector< Node >& lemmas );
    /** get the current fairness literal */
    Node getCurrentFairnessLiteral( TheoryDatatypes * d, std::vector< Node >& lemmas ) { 
      return getFairnessLiteral( d_curr_lit.get(), d, lemmas ); 
    }
    /** increment current term size */
    void incrementCurrentLiteral() { d_curr_lit.set( d_curr_lit.get() + 1 ); }

   private:
    /** the measure value */
    Node d_measure_value;
    /** the sygus measure value */
    Node d_measure_value_active;
  };
  /** the above information for each registered measure term */
  std::map< Node, SearchSizeInfo * > d_szinfo;
  /** map from enumerators (anchors) to their associated measure term */
  std::map< Node, Node > d_anchor_to_measure_term;
  /** map from enumerators (anchors) to their active guard*/
  std::map< Node, Node > d_anchor_to_active_guard;
  /** generic measure term
   *
   * This is a global term that is used as the measure term for all sygus
   * enumerators that do not have active guards. This class enforces that
   * all enumerators have size at most n, where n is the minimal integer
   * such that (DT_SYGUS_BOUND d_generic_measure_term n) is asserted.
   */
  Node d_generic_measure_term;
  /**
   * This increments the current search size for measure term m. This may
   * cause lemmas to be added to lemmas based on the fact that symmetry
   * breaking lemmas are now relevant for new search terms, see discussion
   * of how search size affects which lemmas are relevant above
   * addSymBreakLemmasFor.
   */
  void incrementCurrentSearchSize( Node m, std::vector< Node >& lemmas );
  /**
   * Notify this class that we are currently searching for terms of size at
   * most s as model values for measure term m. Literal exp corresponds to the
   * explanation of why the measure term has size at most n. This calls
   * incrementSearchSize above, until the total number of times we have called
   * incrementSearchSize so far is at least s.
   */
  void notifySearchSize( Node m, unsigned s, Node exp, std::vector< Node >& lemmas );
  /** Allocates a SearchSizeInfo object in d_szinfo. */
  void registerMeasureTerm( Node m );
  /**
   * Return the current search size for arbitrary term n. This is the current
   * search size of the anchor of n.
   */
  unsigned getSearchSizeFor( Node n );
  /** return the current search size for enumerator (anchor) e */
  unsigned getSearchSizeForAnchor(Node e);
  /** Get the current search size for measure term m in this SAT context. */
  unsigned getSearchSizeForMeasureTerm(Node m);
  /** get current template
   *
   * For debugging. This returns a term that corresponds to the current
   * inferred shape of n. For example, if the testers
   *   is-C1( n ) and is-C2( n.1 )
   * have been asserted where C1 and C2 are binary constructors, then this
   * method may return a term of the form:
   *   C1( C2( x1, x2 ), x3 )
   * for fresh variables x1, x2, x3. The map var_count maintains the variable
   * count for generating these fresh variables.
   */
  Node getCurrentTemplate( Node n, std::map< TypeNode, int >& var_count );
  //----------------------end search size information
  /** check testers
   *
   * This is called when we have a model assignment vn for n, where n is
   * a selector chain applied to an enumerator (a search term). This function
   * ensures that testers have been asserted for each subterm of vn. This is
   * critical for ensuring that the proper steps have been taken by this class
   * regarding whether or not vn is a legal value for n (not greater than the
   * current search size and not a value that can be blocked by symmetry
   * breaking).
   *
   * For example, if vn = +( x(), x() ), then we ensure that the testers
   *   is-+( n ), is-x( n.1 ), is-x( n.2 )
   * have been asserted to this class. If a tester is not asserted for some
   * relevant selector chain S( n ) of n, then we add a lemma L for that
   * selector chain to lemmas, where L is the "splitting lemma" for S( n ), that
   * states that the top symbol of S( n ) must be one of the constructors of
   * its type.
   *
   * Notice that this function is a sanity check. Typically, it should be the
   * case that testers are asserted for all subterms of vn, and hence this
   * method should not ever add anything to lemmas. However, due to its
   * importance, we check this regardless.
   */
  bool checkTesters(Node n, Node vn, int ind, std::vector<Node>& lemmas);
  /**
   * Get the current SAT status of the guard g.
   * In particular, this returns 1 if g is asserted true, -1 if it is asserted
   * false, and 0 if it is not asserted.
   */
  int getGuardStatus( Node g );
};

}
}
}

#endif

