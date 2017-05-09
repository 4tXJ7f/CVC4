/*********************                                                        */
/*! \file theory_bool_rewriter.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Tim King, Dejan Jovanovic, Kshitij Bansal
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2016 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief [[ Add one-line brief description here ]]
 **
 ** [[ Add lengthier description here ]]
 ** \todo document this file
 **/

#include "theory/booleans/theory_bool_rewriter.h"

#include <algorithm>

#include "proof/rewrite_proof_dispatcher.h"

namespace CVC4 {
namespace theory {
namespace booleans {

RewriteResponse TheoryBoolRewriter::postRewrite(TNode node) {
  return preRewriteEx<false>(node, NULL);
}

RewriteResponse TheoryBoolRewriter::postRewrite_(TNode node) {
  return preRewrite(node);
}

Node flattenIfNeeded(TNode n) {
  Kind k = n.getKind();
  if (k != kind::AND && k != kind::OR) {
    return n;
  }

  bool needed = false;
  for (size_t i = 0; i < n.getNumChildren(); i++) {
    if (n[i].getKind() == k) {
      needed = true;
      break;
    }
  }
  if (!needed) {
    return n;
  }

  std::vector<TNode> toProcess;
  toProcess.push_back(n);

  typedef std::vector<TNode> ChildList;
  ChildList childList;

  for (unsigned i = 0; i < toProcess.size(); ++ i) {
    TNode current = toProcess[i];
    for(unsigned j = 0, j_end = current.getNumChildren(); j < j_end; ++ j) {
      TNode child = current[j];
      if(child.getKind() == k) {
        toProcess.push_back(child);
      } else {
        childList.push_back(child);
      }
    }
  }

  NodeManager* nm = NodeManager::currentNM();
  return nm->mkNode(k, childList);
}


template<bool Proof>
RewriteResponse TheoryBoolRewriter::postRewriteEx(TNode node_, RewriteProof* proof) {
  Node node = flattenIfNeeded(node_);
  RewriteResponse r = bool_applyRules<Proof>(node, proof);
  /*if (r.node != preRewrite(node).node) {
    std::cout << node << " --> " << r.node << std::endl;
    std::cout << preRewrite(node).node << std::endl;
  }*/
  return r;

}

/**
 * flattenNode looks for children of same kind, and if found merges
 * them into the parent.
 *
 * It simultaneously handles a couple of other optimizations: 
 * - trivialNode - if found during exploration, return that node itself
 *    (like in case of OR, if "true" is found, makes sense to replace
 *     whole formula with "true")
 * - skipNode - as name suggests, skip them
 *    (like in case of OR, you may want to skip any "false" nodes found)
 *
 * Use a null node if you want to ignore any of the optimizations.
 */
RewriteResponse flattenNode(TNode n, TNode trivialNode, TNode skipNode)
{
  typedef std::hash_set<TNode, TNodeHashFunction> node_set;

  node_set visited;
  visited.insert(skipNode);

  std::vector<TNode> toProcess;
  toProcess.push_back(n);

  Kind k = n.getKind();
  typedef std::vector<TNode> ChildList;
  ChildList childList;   //TNode should be fine, since 'n' is still there

  for (unsigned i = 0; i < toProcess.size(); ++ i) {
    TNode current = toProcess[i];
    for(unsigned j = 0, j_end = current.getNumChildren(); j < j_end; ++ j) {
      TNode child = current[j];
      if(visited.find(child) != visited.end()) {
        continue;
      } else if(child == trivialNode) {
        return RewriteResponse(REWRITE_DONE, trivialNode);
      } else {
        visited.insert(child);
        if(child.getKind() == k)
          toProcess.push_back(child);
        else
          childList.push_back(child);
      }
    }
  }
  if (childList.size() == 0) return RewriteResponse(REWRITE_DONE, skipNode);
  if (childList.size() == 1) return RewriteResponse(REWRITE_AGAIN, childList[0]);

  /* Trickery to stay under number of children possible in a node */
  NodeManager* nodeManager = NodeManager::currentNM();
  static const unsigned MAX_CHILDREN = (1u << __CVC4__EXPR__NODE_VALUE__NBITS__NCHILDREN ) - 1;
  if (childList.size() < MAX_CHILDREN) {
    Node retNode = nodeManager->mkNode(k, childList);
    return RewriteResponse(REWRITE_DONE, retNode);
  } else {
    Assert(childList.size() < size_t(MAX_CHILDREN) * size_t(MAX_CHILDREN) );
    NodeBuilder<> nb(k);
    ChildList::iterator cur = childList.begin(), next, en = childList.end();
    while( cur != en ) {
      next = min(cur + MAX_CHILDREN, en);
      nb << (nodeManager->mkNode(k, ChildList(cur, next) ));
      cur = next;
    }
    return RewriteResponse(REWRITE_DONE, nb.constructNode());
  }
}

// Equality parity returns
// * 0 if no relation between a and b is found
// * 1 if a == b
// * 2 if a == not(b)
// * 3 or b == not(a)
inline int equalityParity(TNode a, TNode b){
  if(a == b){
    return 1;
  }else if(a.getKind() == kind::NOT && a[0] == b){
    return 2;
  }else if(b.getKind() == kind::NOT && b[0] == a){
    return 3;
  }else{
    return 0;
  }
}

inline Node makeNegation(TNode n){
  bool even = false;
  while(n.getKind() == kind::NOT){
    n = n[0];
    even = !even;
  }
  if(even){
    return n;
  } else {
    if(n.isConst()){
      return NodeManager::currentNM()->mkConst(!n.getConst<bool>());
    }else{
      return n.notNode();
    }
  }
}

RewriteResponse TheoryBoolRewriter::preRewrite(TNode node) {
  return preRewriteEx<false>(node, NULL);
}

RewriteResponse TheoryBoolRewriter::preRewrite_(TNode n) {
  NodeManager* nodeManager = NodeManager::currentNM();
  Node tt = nodeManager->mkConst(true);
  Node ff = nodeManager->mkConst(false);

  switch(n.getKind()) {
  case kind::NOT: {
    if (n[0] == tt) return RewriteResponse(REWRITE_DONE, ff);
    if (n[0] == ff) return RewriteResponse(REWRITE_DONE, tt);
    if (n[0].getKind() == kind::NOT) return RewriteResponse(REWRITE_AGAIN, n[0][0]);
    break;
  }
  case kind::OR: {
    bool done = true;
    TNode::iterator i = n.begin(), iend = n.end();
    for(; i != iend; ++i) {
      if (*i == tt) return RewriteResponse(REWRITE_DONE, tt);
      if (*i == ff) done = false;
      if ((*i).getKind() == kind::OR) done = false;
    }
    if (!done) {
      return flattenNode(n, /* trivialNode = */ tt, /* skipNode = */ ff);
    }
    break;
  }
  case kind::AND: {
    bool done = true;
    TNode::iterator i = n.begin(), iend = n.end();
    for(; i != iend; ++i) {
      if (*i == ff) return RewriteResponse(REWRITE_DONE, ff);
      if (*i == tt) done = false;
      if ((*i).getKind() == kind::AND) done = false;
    }
    if (!done) {
      RewriteResponse ret = flattenNode(n, /* trivialNode = */ ff, /* skipNode = */ tt);
      Debug("bool-flatten") << n << ": " << ret.node << std::endl;
      return ret;
    }
    break;
  }
  case kind::IMPLIES: {
    if (n[0] == ff || n[1] == tt) return RewriteResponse(REWRITE_DONE, tt);
    if (n[0] == tt && n[1] == ff) return RewriteResponse(REWRITE_DONE, ff);
    if (n[0] == tt) return RewriteResponse(REWRITE_AGAIN, n[1]);
    if (n[1] == ff) return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
    break;
  }
  case kind::EQUAL: {
    // rewrite simple cases of IFF
    if(n[0] == tt) {
      // IFF true x
      return RewriteResponse(REWRITE_AGAIN, n[1]);
    } else if(n[1] == tt) {
      // IFF x true
      return RewriteResponse(REWRITE_AGAIN, n[0]);
    } else if(n[0] == ff) {
      // IFF false x
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[1]));
    } else if(n[1] == ff) {
      // IFF x false
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
    } else if(n[0] == n[1]) {
      // IFF x x
      return RewriteResponse(REWRITE_DONE, tt);
    } else if(n[0].getKind() == kind::NOT && n[0][0] == n[1]) {
      // IFF (NOT x) x
      return RewriteResponse(REWRITE_DONE, ff);
    } else if(n[1].getKind() == kind::NOT && n[1][0] == n[0]) {
      // IFF x (NOT x)
      return RewriteResponse(REWRITE_DONE, ff);
    } else if(n[0].getKind() == kind::EQUAL && n[1].getKind() == kind::EQUAL) {
      // a : (= i x)
      // i : (= j k)
      // x : (= y z)

      // assume wlog k, z are constants and j is the same symbol as y
      // (iff (= j k) (= j z))
      // if k = z
      //  then (iff (= j k) (= j k)) => true
      // else
      //  (iff (= j k) (= j z)) <=> b
      //  b : (and (not (= j k)) (not (= j z)))
      //  (= j k) (= j z) | a b
      //  f       f       | t t
      //  f       t       | f f
      //  t       f       | f f
      //  t       t       | * f
      // * j cannot equal both k and z in a theory model
      TNode t,c;
      if (n[0][0].isConst()) {
        c = n[0][0];
        t = n[0][1];
      }
      else if (n[0][1].isConst()) {
        c = n[0][1];
        t = n[0][0];
      }
      bool matchesForm = false;
      bool constantsEqual = false;
      if (!c.isNull()) {
        if (n[1][0] == t && n[1][1].isConst()) {
          matchesForm = true;
          constantsEqual = (n[1][1] == c);
        }
        else if (n[1][1] == t && n[1][0].isConst()) {
          matchesForm = true;
          constantsEqual = (n[1][0] == c);
        }
      }
      if(matchesForm){
        if(constantsEqual){
          return RewriteResponse(REWRITE_DONE, tt);
        }else{
          Cardinality kappa = t.getType().getCardinality();
          Cardinality two(2l);
          if(kappa.knownLessThanOrEqual(two)){
            return RewriteResponse(REWRITE_DONE, ff);
          }else{
            Node neitherEquality = (makeNegation(n[0])).andNode(makeNegation(n[1]));
            return RewriteResponse(REWRITE_AGAIN, neitherEquality);
          }
        }
      }
    }
    break;
  }
  case kind::XOR: {
    // rewrite simple cases of XOR
    if(n[0] == tt) {
      // XOR true x
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[1]));
    } else if(n[1] == tt) {
      // XOR x true
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
    } else if(n[0] == ff) {
      // XOR false x
      return RewriteResponse(REWRITE_AGAIN, n[1]);
    } else if(n[1] == ff) {
      // XOR x false
      return RewriteResponse(REWRITE_AGAIN, n[0]);
    } else if(n[0] == n[1]) {
      // XOR x x
      return RewriteResponse(REWRITE_DONE, ff);
    } else if(n[0].getKind() == kind::NOT && n[0][0] == n[1]) {
      // XOR (NOT x) x
      return RewriteResponse(REWRITE_DONE, tt);
    } else if(n[1].getKind() == kind::NOT && n[1][0] == n[0]) {
      // XOR x (NOT x)
      return RewriteResponse(REWRITE_DONE, tt);
    }
    break;
  }
  case kind::ITE: {
    // non-Boolean-valued ITEs should have been removed in place of
    // a variable
    // rewrite simple cases of ITE
    if (n[0].isConst()) {
      if (n[0] == tt) {
        // ITE true x y

        Debug("bool-ite") << "n[0] == tt " << n << ": " << n[1] << std::endl;
        return RewriteResponse(REWRITE_AGAIN, n[1]);
      } else {
        Assert(n[0] == ff);
        // ITE false x y
        Debug("bool-ite") << "n[0] == ff " << n << ": " << n[1] << std::endl;
        return RewriteResponse(REWRITE_AGAIN, n[2]);
      }
    } else if (n[1].isConst()) {
      if (n[1] == tt && n[2] == ff) {
        Debug("bool-ite") << "n[1] == tt && n[2] == ff " << n << ": " << n[0] << std::endl;
        return RewriteResponse(REWRITE_AGAIN, n[0]);
      }
      else if (n[1] == ff && n[2] == tt) {
        Debug("bool-ite") << "n[1] == ff && n[2] == tt " << n << ": " << n[0].notNode() << std::endl;
        return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
      }
      // else if(n[1] == ff){
      //   Node resp = (n[0].notNode()).andNode(n[2]);
      //   return RewriteResponse(REWRITE_AGAIN, resp);
      // }
    }
    // else if (n[2].isConst()) {
    //   if(n[2] == ff){
    //     Node resp = (n[0]).andNode(n[1]);
    //     return RewriteResponse(REWRITE_AGAIN, resp);
    //   }
    // }

    int parityTmp;
    if ((parityTmp = equalityParity(n[1], n[2])) != 0) {
      Node resp = (parityTmp == 1) ? (Node)n[1] : n[0].eqNode(n[1]);
      Debug("bool-ite") << "equalityParity n[1], n[2] " << parityTmp
                        << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    // Curiously, this rewrite affects several benchmarks dramatically, including copy_array and some simple_startup - disable for now
    // } else if (n[0].getKind() == kind::NOT) {
    //   return RewriteResponse(REWRITE_AGAIN, n[0][0].iteNode(n[2], n[1]));
    } else if(!n[1].isConst() && (parityTmp = equalityParity(n[0], n[1])) != 0){
      // (parityTmp == 1) if n[0] == n[1]
      // otherwise, n[0] == not(n[1]) or not(n[0]) == n[1]

      // if n[1] is constant this can loop, this is possible in prewrite
      Node resp = n[0].iteNode( (parityTmp == 1) ? tt : ff, n[2]);
      Debug("bool-ite") << "equalityParity n[0], n[1] " << parityTmp
                        << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(!n[2].isConst() && (parityTmp = equalityParity(n[0], n[2])) != 0){
      // (parityTmp == 1) if n[0] == n[2]
      // otherwise, n[0] == not(n[2]) or not(n[0]) == n[2]
      Node resp = n[0].iteNode(n[1], (parityTmp == 1) ? ff : tt);
      Debug("bool-ite") << "equalityParity n[0], n[2] " << parityTmp
                        << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(n[1].getKind() == kind::ITE &&
              (parityTmp = equalityParity(n[0], n[1][0])) != 0){
      // (parityTmp == 1) then n : (ite c (ite c x y) z)
      // (parityTmp > 1)  then n : (ite c (ite (not c) x y) z) or
      // n: (ite (not c) (ite c x y) z)
      Node resp = n[0].iteNode((parityTmp == 1) ? n[1][1] : n[1][2], n[2]);
      Debug("bool-ite") << "equalityParity n[0], n[1][0] " << parityTmp
                        << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(n[2].getKind() == kind::ITE &&
              (parityTmp = equalityParity(n[0], n[2][0])) != 0){
      // (parityTmp == 1) then n : (ite c x (ite c y z))
      // (parityTmp > 1)  then n : (ite c x (ite (not c) y z)) or
      // n: (ite (not c) x (ite c y z))
      Node resp = n[0].iteNode(n[1], (parityTmp == 1) ? n[2][2] : n[2][1]);
      Debug("bool-ite") << "equalityParity n[0], n[2][0] " << parityTmp
                        << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    }
    break;
  }
  default:
    return RewriteResponse(REWRITE_DONE, n);
  }
  return RewriteResponse(REWRITE_DONE, n);
}

template<bool Proof>
RewriteResponse TheoryBoolRewriter::preRewriteEx(TNode node_, RewriteProof* proof) {
  Node node = flattenIfNeeded(node_);
  RewriteResponse r = bool_applyRules<Proof>(node, NULL);
  /*if (r.node != preRewrite_(node).node) {
    std::cout << node << " --> " << r.node << std::endl;
    std::cout << preRewrite_(node).node << std::endl;
  }*/
  return preRewrite_(node_);
}

void TheoryBoolRewriter::printRewriteProof(bool use_cache,
                                           TheoryProofEngine* tp,
                                           const Rewrite* rewrite,
                                           std::ostream& os,
                                           ProofLetMap& globalLetMap) {
  if (rewrite->d_tag == ORIGINAL_OP) {
    if (rewrite->d_children.size() == 0) {
      os << "(iff_symm ";
      tp->printTheoryTerm(rewrite->d_original.toExpr(), os, globalLetMap);
      os << ")";
    } else {
      // XXX: we only need one of those options for each arity
      switch (rewrite->d_original.getKind()) {
        case kind::NOT:
          os << "(symm_formula_op1 not _ _ ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
          os << ")";
          break;
        case kind::IMPLIES:
          os << "(symm_formula_op2 impl _ _ _ _ ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
          os << " ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[1], os, globalLetMap);
          os << ")";
          break;
        case kind::AND:
          os << "(symm_formula_op2 and _ _ _ _ ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
          os << " ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[1], os, globalLetMap);
          os << ")";
          break;
        // XXX: CHECK THIS
        case kind::EQUAL:
          os << "(symm_formula_op2 iff _ _ _ _ ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
          os << " ";
          callPrintRewriteProof(use_cache, tp, rewrite->d_children[1], os, globalLetMap);
          os << ")";
          break;
        default:
          std::cout << "IMPLEMENTATION MISSING " << rewrite->d_original.getKind() << std::endl;
          Unreachable();
      }
    }
  } else if (rewrite->d_tag == SWAP) {
    os << "(do_swap_op2 ";
    switch (rewrite->d_original.getKind()) {
      case kind::EQUAL:
        os << "iff ";
        break;
      case kind::AND:
        os << "and ";
        break;
      case kind::OR:
        os << "or ";
        break;
      case kind::XOR:
        os << "xor ";
        break;
      default:
        std::cout << "IMPLEMENTATION MISSING " << rewrite->d_original.getKind() << std::endl;
        Unreachable();
    }
    os << rewrite->swap_id1 << " ";
    os << rewrite->swap_id2 << " ";
    os << "_ _ _ ";
    callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
    os << ")";
  } else {
    bool_printProof(use_cache, tp, rewrite, os, globalLetMap);
  }
  /*
  if (rewrite->d_tag == NOT_TRUE) {
    os << "(not_t _ ";
    callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
    os << ")";
  } else if (rewrite->d_tag == TRUE_IMPLIES_FALSE) {
    os << "(t_impl_f _ ";
    callPrintRewriteProof(use_cache, tp, rewrite->d_children[0], os, globalLetMap);
    os << ")";
  } else {
    std::cout << "ERROR" << std::endl;
    Unreachable();
  }*/
}

template RewriteResponse TheoryBoolRewriter::preRewriteEx<true>(TNode node, RewriteProof* proof);
template RewriteResponse TheoryBoolRewriter::preRewriteEx<false>(TNode node, RewriteProof* proof);
template RewriteResponse TheoryBoolRewriter::postRewriteEx<true>(TNode node, RewriteProof* proof);
template RewriteResponse TheoryBoolRewriter::postRewriteEx<false>(TNode node, RewriteProof* proof);

}/* CVC4::theory::booleans namespace */
}/* CVC4::theory namespace */
}/* CVC4 namespace */
