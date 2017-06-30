/*********************                                                        */
/*! \file solve_real_as_int.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **  Justin Xu
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2017 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Preprocessing pass for solving reals as ints
 ** Preprocessing pass for solving reals as ints.
 **/
#include "preproc/solve_real_as_int.h"
#include "theory/quantifiers/quant_util.h"

namespace CVC4 {
namespace preproc {

SolveRealAsIntPass::SolveRealAsIntPass(PreprocessingPassRegistry* registry)
    : PreprocessingPass(registry, "solveRealAsInt", true) {}

PreprocessingPassResult SolveRealAsIntPass::applyInternal(
    AssertionPipeline* assertionsToPreprocess) {
  std::unordered_map<Node, Node, NodeHashFunction> cache;
  std::vector<Node> var_eq;
  for (unsigned i = 0; i < assertionsToPreprocess->size(); ++i) {
    assertionsToPreprocess->replace(
        i, realToInt((*assertionsToPreprocess)[i], cache, var_eq));
  }
  /* these comments were here before
   if( !var_eq.empty() ){
     unsigned lastIndex = d_assertions.size()-1;
     var_eq.insert( var_eq.begin(), d_assertions[lastIndex] );
     d_assertions.replace(last_index, NodeManager::currentNM()->mkNode(
   kind::AND, var_eq ) );
   }
   */
  return NO_CONFLICT;
}

Node SolveRealAsIntPass::realToInt(TNode n, NodeMap& cache,
                                   std::vector<Node>& var_eq) {
  Trace("real-as-int-debug") << "Convert : " << n << std::endl;
  NodeMap::iterator find = cache.find(n);
  if (find != cache.end()) {
    return (*find).second;
  } else {
    Node ret = n;
    if (n.getNumChildren() > 0) {
      if (n.getKind() == kind::EQUAL || n.getKind() == kind::GEQ ||
          n.getKind() == kind::LT || n.getKind() == kind::GT ||
          n.getKind() == kind::LEQ) {
        ret = theory::Rewriter::rewrite(n);
        Trace("real-as-int-debug") << "Now looking at : " << ret << std::endl;
        if (!ret.isConst()) {
          Node ret_lit = ret.getKind() == kind::NOT ? ret[0] : ret;
          bool ret_pol = ret.getKind() != kind::NOT;
          std::map<Node, Node> msum;
          if (theory::QuantArith::getMonomialSumLit(ret_lit, msum)) {
            // get common coefficient
            std::vector<Node> coeffs;
            for (std::map<Node, Node>::iterator itm = msum.begin();
                 itm != msum.end(); ++itm) {
              Node v = itm->first;
              Node c = itm->second;
              if (!c.isNull()) {
                Assert(c.isConst());
                coeffs.push_back(NodeManager::currentNM()->mkConst(
                    Rational(c.getConst<Rational>().getDenominator())));
              }
            }
            Node cc = coeffs.empty()
                          ? Node::null()
                          : (coeffs.size() == 1
                                 ? coeffs[0]
                                 : theory::Rewriter::rewrite(
                                       NodeManager::currentNM()->mkNode(
                                           kind::MULT, coeffs)));
            std::vector<Node> sum;
            for (std::map<Node, Node>::iterator itm = msum.begin();
                 itm != msum.end(); ++itm) {
              Node v = itm->first;
              Node c = itm->second;
              Node s;
              if (c.isNull()) {
                c = cc.isNull() ? NodeManager::currentNM()->mkConst(Rational(1))
                                : cc;
              } else {
                if (!cc.isNull()) {
                  c = theory::Rewriter::rewrite(
                      NodeManager::currentNM()->mkNode(kind::MULT, c, cc));
                }
              }
              Assert(c.getType().isInteger());
              if (v.isNull()) {
                sum.push_back(c);
              } else {
                Node vv = realToInt(v, cache, var_eq);
                if (vv.getType().isInteger()) {
                  sum.push_back(
                      NodeManager::currentNM()->mkNode(kind::MULT, c, vv));
                } else {
                  throw TypeCheckingException(
                      v.toExpr(),
                      std::string("Cannot translate to Int: ") + v.toString());
                }
              }
            }
            Node sumt =
                sum.empty()
                    ? NodeManager::currentNM()->mkConst(Rational(0))
                    : (sum.size() == 1
                           ? sum[0]
                           : NodeManager::currentNM()->mkNode(kind::PLUS, sum));
            ret = NodeManager::currentNM()->mkNode(
                ret_lit.getKind(), sumt,
                NodeManager::currentNM()->mkConst(Rational(0)));
            if (!ret_pol) {
              ret = ret.negate();
            }
            Trace("real-as-int") << "Convert : " << std::endl;
            Trace("real-as-int") << "   " << n << std::endl;
            Trace("real-as-int") << "   " << ret << std::endl;
          } else {
            throw TypeCheckingException(
                n.toExpr(),
                std::string("Cannot translate to Int: ") + n.toString());
          }
        }
      } else {
        bool childChanged = false;
        std::vector<Node> children;
        for (unsigned i = 0; i < n.getNumChildren(); i++) {
          Node nc = realToInt(n[i], cache, var_eq);
          childChanged = childChanged || nc != n[i];
          children.push_back(nc);
        }
        if (childChanged) {
          ret = NodeManager::currentNM()->mkNode(n.getKind(), children);
        }
      }
    } else {
      if (n.isVar()) {
        if (!n.getType().isInteger()) {
          ret = NodeManager::currentNM()->mkSkolem(
              "__realToInt_var", NodeManager::currentNM()->integerType(),
              "Variable introduced in realToInt pass");
          var_eq.push_back(n.eqNode(ret));
        }
      }
    }
    cache[n] = ret;
    return ret;
  }
}

}  // namespace preproc
}  // namespace CVC4
