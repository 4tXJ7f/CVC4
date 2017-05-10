/*********************                                                        */
/*! \file rewriter.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Dejan Jovanovic, Morgan Deters, Liana Hadarean
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

#include "theory/rewriter.h"

#include "proof/rewrite_proof_dispatcher.h"
#include "theory/rewriter_tables.h"

using namespace std;

namespace CVC4 {
namespace theory {

unsigned long Rewriter::d_iterationCount = 0;

#ifdef CVC4_ASSERTIONS
static CVC4_THREADLOCAL(std::hash_set<Node, NodeHashFunction>*) s_rewriteStack = NULL;
#endif /* CVC4_ASSERTIONS */

class RewriterInitializer {
  static RewriterInitializer s_rewriterInitializer;
  RewriterInitializer() {
    Rewriter::init();
  }
  ~RewriterInitializer() { Rewriter::shutdown(); }
};/* class RewriterInitializer */

/**
 * This causes initialization of the rewriter before first use,
 * and tear-down at exit time.
 */
RewriterInitializer RewriterInitializer::s_rewriterInitializer;

/**
 * TheoryEngine::rewrite() keeps a stack of things that are being pre-
 * and post-rewritten.  Each element of the stack is a
 * RewriteStackElement.
 */
struct RewriteStackElement {

  /** The node we're currently rewriting */
  Node node;
  /** Original node */
  Node original;
  /** Id of the theory that's currently rewriting this node */
  unsigned theoryId         : 8;
  /** Id of the original theory that started the rewrite */
  unsigned originalTheoryId : 8;
  /** Index of the child this node is done rewriting */
  unsigned nextChild        : 32;
  /** Builder for this node */
  NodeBuilder<> builder;

  /**
   * Construct a fresh stack element.
   */
  RewriteStackElement(TNode node, TheoryId theoryId) :
    node(node),
    original(node),
    theoryId(theoryId),
    originalTheoryId(theoryId),
    nextChild(0) {
  }
};

Node Rewriter::rewrite(TNode node) {
  return rewriteTo<false>(theoryOf(node), node, NULL);
}

template<bool Proof>
Node Rewriter::rewriteTo(theory::TheoryId theoryId, Node node, RewriteProof* rp) {
#ifdef CVC4_ASSERTIONS
  bool isEquality = node.getKind() == kind::EQUAL && (!node[0].getType().isBoolean());

  if(s_rewriteStack == NULL) {
    s_rewriteStack = new std::hash_set<Node, NodeHashFunction>();
  }
#endif

  Trace("rewriter") << "Rewriter::rewriteTo(" << theoryId << "," << node << ")"<< std::endl;

  // Check if it's been cached already
  if (!Proof) {
    Node cached = getPostRewriteCache(theoryId, node);
    if (!cached.isNull()) {
      return cached;
    }
  }

  // Put the node on the stack in order to start the "recursive" rewrite
  vector<RewriteStackElement> rewriteStack;
  rewriteStack.push_back(RewriteStackElement(node, theoryId));
  if (Proof) {
    rp->addRewrite(new Rewrite(ORIGINAL_OP, node));
  }

  ResourceManager* rm = NULL;
  bool hasSmtEngine = smt::smtEngineInScope();
  if (hasSmtEngine) {
    rm = NodeManager::currentResourceManager();
  }
  // Rewrite until the stack is empty
  for (;;){

    if (hasSmtEngine &&
    d_iterationCount % ResourceManager::getFrequencyCount() == 0) {
      rm->spendResource(options::rewriteStep());
      d_iterationCount = 0;
    }

    // Get the top of the recursion stack
    RewriteStackElement& rewriteStackTop = rewriteStack.back();
    Trace("rewriter") << "Rewriter::rewriting: " << (TheoryId) rewriteStackTop.theoryId << "," << rewriteStackTop.node << std::endl;

    // Before rewriting children we need to do a pre-rewrite of the node
    if (rewriteStackTop.nextChild == 0) {
      // Check if the pre-rewrite has already been done (it's in the cache)
      Node cached = Rewriter::getPreRewriteCache((TheoryId) rewriteStackTop.theoryId, rewriteStackTop.node);
      if ((Proof && rp->getPreRewriteCache(rewriteStackTop.node) == NULL) || cached.isNull()) {
        // Rewrite until fix-point is reached
        for(;;) {
          // Perform the pre-rewrite
          RewriteResponse response = Proof ? callPreRewriteWithProof((TheoryId) rewriteStackTop.theoryId, rewriteStackTop.node, rp) : Rewriter::callPreRewrite((TheoryId) rewriteStackTop.theoryId, rewriteStackTop.node);

          // Put the rewritten node to the top of the stack
          rewriteStackTop.node = response.node;
          TheoryId newTheory = theoryOf(rewriteStackTop.node);
          // In the pre-rewrite, if changing theories, we just call the other theories pre-rewrite
          if (newTheory == (TheoryId) rewriteStackTop.theoryId && response.status == REWRITE_DONE) {
            break;
          }
          rewriteStackTop.theoryId = newTheory;
        }

        // Cache the rewrite
        Rewriter::setPreRewriteCache((TheoryId) rewriteStackTop.originalTheoryId, rewriteStackTop.original, rewriteStackTop.node);
        if (Proof) {
          rp->setPreRewriteCache(rewriteStackTop.original, rp->getTopRewrite());
        }
      }
      // Otherwise we're have already been pre-rewritten (in pre-rewrite cache)
      else {
        // Continue with the cached version
        rewriteStackTop.node = cached;
        rewriteStackTop.theoryId = theoryOf(cached);
        if (Proof) {
          Rewrite* cachedRewrite = rp->getPreRewriteCache(rewriteStackTop.original);
          cachedRewrite->d_cached_version_used = true;
          rp->replaceRewrite(cachedRewrite);
        }
      }
    }

    rewriteStackTop.original =rewriteStackTop.node;
    // Now it's time to rewrite the children, check if this has already been done
    Node cached = Rewriter::getPostRewriteCache((TheoryId) rewriteStackTop.theoryId, rewriteStackTop.node);
    // If not, go through the children
    if((Proof && rp->getPostRewriteCache(rewriteStackTop.node) == NULL) || cached.isNull()) {

      // The child we need to rewrite
      unsigned child = rewriteStackTop.nextChild++;

      // To build the rewritten expression we set up the builder
      if(child == 0) {
        if (rewriteStackTop.node.getNumChildren() > 0) {
          // The children will add themselves to the builder once they're done
          rewriteStackTop.builder << rewriteStackTop.node.getKind();
          kind::MetaKind metaKind = rewriteStackTop.node.getMetaKind();
          if (metaKind == kind::metakind::PARAMETERIZED) {
            rewriteStackTop.builder << rewriteStackTop.node.getOperator();
          }
        }
      }

      // Process the next child
      if(child < rewriteStackTop.node.getNumChildren()) {
        // The child node
        Node childNode = rewriteStackTop.node[child];
        // Push the rewrite request to the stack (NOTE: rewriteStackTop might be a bad reference now)
        rewriteStack.push_back(RewriteStackElement(childNode, theoryOf(childNode)));
        if (Proof) {
          rp->addRewrite(new Rewrite(ORIGINAL_OP, childNode));
        }
        // Go on with the rewriting
        continue;
      }

      // Incorporate the children if necessary
      if (rewriteStackTop.node.getNumChildren() > 0) {
        Node rewritten = rewriteStackTop.builder;
        rewriteStackTop.node = rewritten;
        rewriteStackTop.theoryId = theoryOf(rewriteStackTop.node);
      }

      // Done with all pre-rewriting, so let's do the post rewrite
      for(;;) {
        // Do the post-rewrite
        RewriteResponse response = Proof ? callPostRewriteWithProof((TheoryId) rewriteStackTop.theoryId, rewriteStackTop.node, rp) : Rewriter::callPostRewrite((TheoryId) rewriteStackTop.theoryId, rewriteStackTop.node);
        // We continue with the response we got
        TheoryId newTheoryId = theoryOf(response.node);
        if (newTheoryId != (TheoryId) rewriteStackTop.theoryId || response.status == REWRITE_AGAIN_FULL) {
          // In the post rewrite if we've changed theories, we must do a full rewrite
          Assert(response.node != rewriteStackTop.node);
          //TODO: this is not thread-safe - should make this assertion dependent on sequential build
#ifdef CVC4_ASSERTIONS
          Assert(s_rewriteStack->find(response.node) == s_rewriteStack->end());
          s_rewriteStack->insert(response.node);
#endif
          if (Proof) {
            RewriteProof subRp;
            Node rewritten = rewriteWithProof(response.node, &subRp);
            rewriteStackTop.node = rewritten;
          } else {
            rewriteStackTop.node = rewrite(response.node);
          }
#ifdef CVC4_ASSERTIONS
          Assert(s_rewriteStack);
          s_rewriteStack->erase(response.node);
#endif
          break;
        } else if (response.status == REWRITE_DONE) {
#ifdef CVC4_ASSERTIONS
    RewriteResponse r2 = Rewriter::callPostRewrite(newTheoryId, response.node);
    if (r2.node != response.node) {
      std::cout << rewriteStackTop.node << std::endl;
      std::cout << r2.node << " IS NOT EQUAL TO " << response.node << std::endl;
    }
    Assert(r2.node == response.node);
#endif
          rewriteStackTop.node = response.node;
          break;
        }
        // Check for trivial rewrite loops of size 1 or 2
        Assert(response.node != rewriteStackTop.node);
        Assert(Rewriter::callPostRewrite((TheoryId) rewriteStackTop.theoryId, response.node).node != rewriteStackTop.node);
        rewriteStackTop.node = response.node;
      }
      // We're done with the post rewrite, so we add to the cache
      Rewriter::setPostRewriteCache((TheoryId) rewriteStackTop.originalTheoryId, rewriteStackTop.original, rewriteStackTop.node);

    } else {
      // We were already in cache, so just remember it
      rewriteStackTop.node = cached;
      rewriteStackTop.theoryId = theoryOf(cached);
      if (Proof) {
        Rewrite* cachedRewrite = rp->getPostRewriteCache(rewriteStackTop.original);
        cachedRewrite->d_cached_version_used = true;
        rp->replaceRewrite(cachedRewrite);
      }
    }

    // If this is the last node, just return
    if (rewriteStack.size() == 1) {
      Assert(!isEquality || rewriteStackTop.node.getKind() == kind::EQUAL || rewriteStackTop.node.isConst());
      return rewriteStackTop.node;
    }

    // We're done with this node, append it to the parent
    rewriteStack[rewriteStack.size() - 2].builder << rewriteStackTop.node;
    rewriteStack.pop_back();

    if (Proof) {
      rp->attachSubproofToParent();
    }
  }

  Unreachable();
  return Node::null();
}/* Rewriter::rewriteTo() */

Node Rewriter::rewriteWithProof(TNode node, RewriteProof* rp) throw (UnsafeInterruptException){
  Node result = rewriteTo<true>(theoryOf(node), node, rp);
  return result;
}

void Rewriter::clearCaches() {
#ifdef CVC4_ASSERTIONS
  if(s_rewriteStack != NULL) {
    delete s_rewriteStack;
    s_rewriteStack = NULL;
  }
#endif
  Rewriter::clearCachesInternal();
}

template Node Rewriter::rewriteTo<true>(theory::TheoryId theoryId, Node node, RewriteProof* rp);
template Node Rewriter::rewriteTo<false>(theory::TheoryId theoryId, Node node, RewriteProof* rp);

}/* CVC4::theory namespace */
}/* CVC4 namespace */
