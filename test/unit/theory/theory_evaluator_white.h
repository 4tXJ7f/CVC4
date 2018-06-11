/*********************                                                        */
/*! \file theory_evaluator_white.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Andres Noetzli
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2017 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief [[ Add one-line brief description here ]]
 **
 ** [[ Add lengthier description here ]]
 ** \todo document this file
 **/

#include <cxxtest/TestSuite.h>
#include <vector>

#include "expr/node.h"
#include "expr/node_manager.h"
#include "smt/smt_engine.h"
#include "smt/smt_engine_scope.h"
#include "theory/evaluator.h"
#include "theory/rewriter.h"
#include "theory/strings/theory_strings_rewriter.h"
#include "theory/theory_test_utils.h"

using namespace CVC4;
using namespace CVC4::smt;
using namespace CVC4::theory;

using namespace std;

class TheoryEvaluatorWhite : public CxxTest::TestSuite
{
  ExprManager *d_em;
  NodeManager *d_nm;
  SmtEngine *d_smt;
  SmtScope *d_scope;

 public:
  TheoryEvaluatorWhite() {}

  void setUp()
  {
    Options opts;
    opts.setOutputLanguage(language::output::LANG_SMTLIB_V2);
    d_em = new ExprManager(opts);
    d_nm = NodeManager::fromExprManager(d_em);
    d_smt = new SmtEngine(d_em);
    d_scope = new SmtScope(d_smt);
  }

  void tearDown()
  {
    delete d_scope;
    delete d_smt;
    delete d_em;
  }

  void testCheckEntailArithWithAssumption()
  {
    TypeNode bv64Type = d_nm->mkBitVectorType(64);

    Node w = d_nm->mkVar("w", bv64Type);
    Node x = d_nm->mkVar("x", bv64Type);
    Node y = d_nm->mkVar("y", bv64Type);
    Node z = d_nm->mkVar("z", bv64Type);

    Node zero = d_nm->mkConst(BitVector(64, (unsigned int)0));
    Node one = d_nm->mkConst(BitVector(64, (unsigned int)1));
    Node c1 = d_nm->mkConst(BitVector(
        64,
        (unsigned int)0b0000000100000101001110111001101000101110011101011011110011100111));
    Node c2 = d_nm->mkConst(BitVector(
        64,
        (unsigned int)0b0000000100000101001110111001101000101110011101011011110011100111));

    std::vector<Node> args = {w, x, y, z};
    std::vector<Node> vals = {c1, zero, one, c1};

    Evaluator eval;
    eval.eval(d_nm->mkNode(kind::ITE, d_nm->mkNode(kind::EQUAL, y, one), x, w),
              args,
              vals);
  }
};
