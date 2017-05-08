/*********************                                                        */
/*! \file rewriter.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Dejan Jovanovic, Morgan Deters, Tim King
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2016 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief The Rewriter class
 **
 ** The Rewriter class.
 **/

#include "cvc4_private.h"

#pragma once

#include "expr/node.h"
#include "smt/smt_engine_scope.h"
#include "smt/smt_statistics_registry.h"
#include "theory/theory.h"
#include "util/resource_manager.h"
#include "util/unsafe_interrupt_exception.h"

using namespace std;

namespace CVC4 {
namespace theory {

static inline Node mkCommutativeNode(Kind k, const std::vector<Node>& children) {
  NodeManager *nm = NodeManager::currentNM();

  if (children.size() == 0) {
    switch (k) {
      case kind::AND:
        return nm->mkConst(true);
        break;
      case kind::OR:
        return nm->mkConst(false);
        break;
      default:
        abort();
        break;
    }
  } else if (children.size() == 1) {
    return children[0];
  } else {
    return nm->mkNode(k, children);
  }
}

static inline TheoryId theoryOf(TNode node) {
  return Theory::theoryOf(THEORY_OF_TYPE_BASED, node);
}

/**
 * Theory rewriters signal whether more rewriting is needed (or not)
 * by using a member of this enumeration.  See RewriteResponse, below.
 */
enum RewriteStatus {
  REWRITE_DONE,
  REWRITE_AGAIN,
  REWRITE_AGAIN_FULL
};/* enum RewriteStatus */

/**
 * Instances of this class serve as response codes from
 * Theory::preRewrite() and Theory::postRewrite().  Instances of
 * derived classes RewriteComplete(n), RewriteAgain(n), and
 * FullRewriteNeeded(n) should be used, giving self-documenting
 * rewrite behavior.
 */
struct RewriteResponse {
  const RewriteStatus status;
  const Node node;

  RewriteResponse(RewriteStatus status, Node node) :
    status(status), node(node) {}
};/* struct RewriteResponse */

class RewriterInitializer;

/**
 * The main rewriter class.  All functionality is static.
 */
class Rewriter {

  friend class RewriterInitializer;
  static unsigned long d_iterationCount;
  /** Returns the appropriate cache for a node */
  static Node getPreRewriteCache(theory::TheoryId theoryId, TNode node);

  /** Returns the appropriate cache for a node */
  static Node getPostRewriteCache(theory::TheoryId theoryId, TNode node);

  /** Sets the appropriate cache for a node */
  static void setPreRewriteCache(theory::TheoryId theoryId,
                                 TNode node, TNode cache);

  /** Sets the appropriate cache for a node */
  static void setPostRewriteCache(theory::TheoryId theoryId,
                                  TNode node, TNode cache);

  // disable construction of rewriters; all functionality is static
  Rewriter() CVC4_UNDEFINED;
  Rewriter(const Rewriter&) CVC4_UNDEFINED;

  /**
   * Rewrites the node using the given theory rewriter.
   */
  template<bool Proof>
  static Node rewriteTo(theory::TheoryId theoryId, Node node, RewriteProof* rp);

  /** Calls the pre-rewriter for the given theory */
  static RewriteResponse callPreRewrite(theory::TheoryId theoryId, TNode node);

  /** Calls the post-rewriter for the given theory */
  static RewriteResponse callPostRewrite(theory::TheoryId theoryId, TNode node);

  /**
   * Calls the equality-rewriter for the given theory.
   */
  static Node callRewriteEquality(theory::TheoryId theoryId, TNode equality);

  /**
   * Should be called before the rewriter gets used for the first time.
   */
  static void init();

  /**
   * Should be called to clean up any state.
   */
  static void shutdown();
  static void clearCachesInternal();
public:

  /**
   * Rewrites the node using theoryOf() to determine which rewriter to
   * use on the node.
   */
  static Node rewrite(TNode node);

  /**
   * Rewrites the node using theoryOf() to determine which rewriter to
   * use on the node and generates a proof.
   */
  static Node rewriteWithProof(TNode node, RewriteProof* rp) throw (UnsafeInterruptException);

  /**
   * Garbage collects the rewrite caches.
   */
  static void clearCaches();
};/* class Rewriter */

}/* CVC4::theory namespace */
}/* CVC4 namespace */
