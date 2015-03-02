/*********************                                                        */
/*! \file ce_guided_single_inv.cpp
 ** \verbatim
 ** Original author: Andrew Reynolds
 ** Major contributors: none
 ** Minor contributors (to current version): none
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2014  New York University and The University of Iowa
 ** See the file COPYING in the top-level source directory for licensing
 ** information.\endverbatim
 **
 ** \brief utility for processing single invocation synthesis conjectures
 **
 **/

#include "theory/quantifiers/ce_guided_single_inv.h"
#include "theory/quantifiers/ce_guided_instantiation.h"
#include "theory/theory_engine.h"
#include "theory/quantifiers/options.h"
#include "theory/quantifiers/term_database.h"
#include "theory/quantifiers/first_order_model.h"
#include "theory/datatypes/datatypes_rewriter.h"
#include "util/datatype.h"
#include "theory/quantifiers/quant_util.h"
#include "theory/quantifiers/trigger.h"

using namespace CVC4;
using namespace CVC4::kind;
using namespace CVC4::theory;
using namespace CVC4::theory::quantifiers;
using namespace std;

namespace CVC4 {

CegConjectureSingleInv::CegConjectureSingleInv( CegConjecture * p ) : d_parent( p ){
  d_sol = NULL;
  d_c_inst_match_trie = NULL;
}

Node CegConjectureSingleInv::getSingleInvLemma( Node guard ) {
  if( !d_single_inv.isNull() ) {
    Assert( d_single_inv.getKind()==FORALL );
    d_single_inv_var.clear();
    d_single_inv_sk.clear();
    for( unsigned i=0; i<d_single_inv[0].getNumChildren(); i++ ){
      std::stringstream ss;
      ss << "k_" << d_single_inv[0][i];
      Node k = NodeManager::currentNM()->mkSkolem( ss.str(), d_single_inv[0][i].getType(), "single invocation function skolem" );
      d_single_inv_var.push_back( d_single_inv[0][i] );
      d_single_inv_sk.push_back( k );
      d_single_inv_sk_index[k] = i;
    }
    Node inst = d_single_inv[1].substitute( d_single_inv_var.begin(), d_single_inv_var.end(), d_single_inv_sk.begin(), d_single_inv_sk.end() );
    inst = TermDb::simpleNegate( inst );
    Trace("cegqi-si") << "Single invocation initial lemma : " << inst << std::endl;
    return NodeManager::currentNM()->mkNode( OR, guard.negate(), inst );
  }else{
    return Node::null();
  }
}

void CegConjectureSingleInv::initialize( QuantifiersEngine * qe, Node q ) {
  //initialize data
  d_sol = new CegConjectureSingleInvSol( qe );
  d_qe = qe;
  d_quant = q;
  if( options::incrementalSolving() ){
    d_c_inst_match_trie = new inst::CDInstMatchTrie( qe->getUserContext() );
  }
  //process
  Trace("cegqi-si") << "Initialize cegqi-si for " << q << std::endl;
  // conj -> conj*
  std::map< Node, std::vector< Node > > children;
  // conj X prog -> inv*
  std::map< Node, std::map< Node, std::vector< Node > > > prog_invoke;
  std::vector< Node > progs;
  std::map< Node, std::map< Node, bool > > contains;
  for( unsigned i=0; i<q[0].getNumChildren(); i++ ){
    progs.push_back( q[0][i] );
  }
  //collect information about conjunctions
  bool singleInvocation = false;
  if( analyzeSygusConjunct( Node::null(), q[1], children, prog_invoke, progs, contains, true ) ){
    singleInvocation = true;
    //try to phrase as single invocation property
    Trace("cegqi-si") << "...success." << std::endl;
    std::map< Node, std::vector< Node > > invocations;
    std::map< Node, std::map< int, std::vector< Node > > > single_invoke_args;
    std::map< Node, std::map< int, std::map< Node, std::vector< Node > > > > single_invoke_args_from;
    for( std::map< Node, std::vector< Node > >::iterator it = children.begin(); it != children.end(); ++it ){
      for( unsigned j=0; j<it->second.size(); j++ ){
        Node conj = it->second[j];
        Trace("cegqi-si-debug") << "Process child " << conj << " from " << it->first << std::endl;
        std::map< Node, std::map< Node, std::vector< Node > > >::iterator itp = prog_invoke.find( conj );
        if( itp!=prog_invoke.end() ){
          for( std::map< Node, std::vector< Node > >::iterator it2 = itp->second.begin(); it2 != itp->second.end(); ++it2 ){
            if( it2->second.size()>1 ){
              singleInvocation = false;
              break;
            }else if( it2->second.size()==1 ){
              Node prog = it2->first;
              Node inv = it2->second[0];
              Assert( inv[0]==prog );
              invocations[prog].push_back( inv );
              for( unsigned k=1; k<inv.getNumChildren(); k++ ){
                Node arg = inv[k];
                Trace("cegqi-si-debug") << "process : " << arg << " occurs in position " << k-1 << " in invocation " << inv << " of " << prog << " in " << conj << std::endl;
                single_invoke_args_from[prog][k-1][arg].push_back( conj );
                if( std::find( single_invoke_args[prog][k-1].begin(), single_invoke_args[prog][k-1].end(), arg )==single_invoke_args[prog][k-1].end() ){
                  single_invoke_args[prog][k-1].push_back( arg );
                }
              }
            }
          }
        }
      }
    }
    if( singleInvocation ){
      Trace("cegqi-si") << "Property is single invocation with : " << std::endl;
      std::vector< Node > pbvs;
      std::vector< Node > new_vars;
      std::map< Node, std::vector< Node > > new_assumptions;
      for( std::map< Node, std::vector< Node > >::iterator it = invocations.begin(); it != invocations.end(); ++it ){
        Assert( !it->second.empty() );
        Node prog = it->first;
        Node inv = it->second[0];
        std::vector< Node > invc;
        invc.push_back( inv.getOperator() );
        invc.push_back( prog );
        std::stringstream ss;
        ss << "F_" << prog;
        Node pv = NodeManager::currentNM()->mkBoundVar( ss.str(), inv.getType() );
        d_single_inv_map[prog] = pv;
        d_single_inv_map_to_prog[pv] = prog;
        pbvs.push_back( pv );
        Trace("cegqi-si-debug2") << "Make variable " << pv << " for " << prog << std::endl;
        for( unsigned k=1; k<inv.getNumChildren(); k++ ){
          Assert( !single_invoke_args[prog][k-1].empty() );
          if( single_invoke_args[prog][k-1].size()==1 && single_invoke_args[prog][k-1][0].getKind()==BOUND_VARIABLE ){
            invc.push_back( single_invoke_args[prog][k-1][0] );
          }else{
            //introduce fresh variable, assign all
            Node v = NodeManager::currentNM()->mkSkolem( "a", single_invoke_args[prog][k-1][0].getType(), "single invocation arg" );
            new_vars.push_back( v );
            invc.push_back( v );
            d_single_inv_arg_sk.push_back( v );

            for( unsigned i=0; i<single_invoke_args[prog][k-1].size(); i++ ){
              Node arg = single_invoke_args[prog][k-1][i];
              Node asum = NodeManager::currentNM()->mkNode( EQUAL, v, arg ).negate();
              Trace("cegqi-si-debug") << "  ..." << arg << " occurs in ";
              Trace("cegqi-si-debug") << single_invoke_args_from[prog][k-1][arg].size() << " invocations at position " << (k-1) << " of " << prog << "." << std::endl;
              for( unsigned j=0; j<single_invoke_args_from[prog][k-1][arg].size(); j++ ){
                Node conj = single_invoke_args_from[prog][k-1][arg][j];
                Trace("cegqi-si-debug") << "  ..." << arg << " occurs in invocation " << inv << " of " << prog << " in " << conj << std::endl;
                Trace("cegqi-si-debug") << "  ...add assumption " << asum << " to " << conj << std::endl;
                if( std::find( new_assumptions[conj].begin(), new_assumptions[conj].end(), asum )==new_assumptions[conj].end() ){
                  new_assumptions[conj].push_back( asum );
                }
              }
            }
          }
        }
        Node sinv = NodeManager::currentNM()->mkNode( APPLY_UF, invc );
        Trace("cegqi-si") << "  " << prog << " -> " << sinv << std::endl;
        d_single_inv_app_map[prog] = sinv;
      }
      //construct the single invocation version of the property
      Trace("cegqi-si") << "Single invocation formula conjuncts are : " << std::endl;
      //std::vector< Node > si_conj;
      Assert( !pbvs.empty() );
      Node pbvl = NodeManager::currentNM()->mkNode( BOUND_VAR_LIST, pbvs );
      for( std::map< Node, std::vector< Node > >::iterator it = children.begin(); it != children.end(); ++it ){
        //should hold since we prevent miniscoping
        Assert( d_single_inv.isNull() );
        std::vector< Node > conjuncts;
        for( unsigned i=0; i<it->second.size(); i++ ){
          Node c = it->second[i];
          std::vector< Node > disj;
          //insert new assumptions
          disj.insert( disj.end(), new_assumptions[c].begin(), new_assumptions[c].end() );
          //get replaced version
          Node cr;
          std::map< Node, std::map< Node, std::vector< Node > > >::iterator itp = prog_invoke.find( c );
          if( itp!=prog_invoke.end() ){
            std::vector< Node > terms;
            std::vector< Node > subs;
            for( std::map< Node, std::vector< Node > >::iterator it2 = itp->second.begin(); it2 != itp->second.end(); ++it2 ){
              if( !it2->second.empty() ){
                Node prog = it2->first;
                Node inv = it2->second[0];
                Assert( it2->second.size()==1 );
                terms.push_back( inv );
                subs.push_back( d_single_inv_map[prog] );
                Trace("cegqi-si-debug2") << "subs : " << inv << " -> var for " << prog << " : " << d_single_inv_map[prog] << std::endl;
              }
            }
            cr = c.substitute( terms.begin(), terms.end(), subs.begin(), subs.end() );
          }else{
            cr = c;
          }
          if( cr.getKind()==OR ){
            for( unsigned j=0; j<cr.getNumChildren(); j++ ){
              disj.push_back( cr[j] );
            }
          }else{
            disj.push_back( cr );
          }
          Node curr = disj.size()==1 ? disj[0] : NodeManager::currentNM()->mkNode( OR, disj );
          curr = TermDb::simpleNegate( curr );
          Trace("cegqi-si") << "    " << curr;
          conjuncts.push_back( curr );
          if( !it->first.isNull() ){
            Trace("cegqi-si-debug") << " under " << it->first;
          }
          Trace("cegqi-si") << std::endl;
        }
        Assert( !conjuncts.empty() );
        //make the skolems for the existential
        if( !it->first.isNull() ){
          std::vector< Node > vars;
          std::vector< Node > sks;
          for( unsigned j=0; j<it->first.getNumChildren(); j++ ){
            std::stringstream ss;
            ss << "a_" << it->first[j];
            Node v = NodeManager::currentNM()->mkSkolem( ss.str(), it->first[j].getType(), "single invocation skolem" );
            vars.push_back( it->first[j] );
            sks.push_back( v );
          }
          //substitute conjunctions
          for( unsigned i=0; i<conjuncts.size(); i++ ){
            conjuncts[i] = conjuncts[i].substitute( vars.begin(), vars.end(), sks.begin(), sks.end() );
          }
          d_single_inv_arg_sk.insert( d_single_inv_arg_sk.end(), sks.begin(), sks.end() );
          //substitute single invocation applications
          for( std::map< Node, Node >::iterator itam = d_single_inv_app_map.begin(); itam != d_single_inv_app_map.end(); ++itam ){
            Node n = itam->second;
            d_single_inv_app_map[itam->first] = n.substitute( vars.begin(), vars.end(), sks.begin(), sks.end() );
          }
        }
        //ensure that this is a ground property
        for( std::map< Node, Node >::iterator itam = d_single_inv_app_map.begin(); itam != d_single_inv_app_map.end(); ++itam ){
          Node n = itam->second;
          //check if all variables are arguments of this
          std::vector< Node > n_args;
          for( unsigned i=1; i<n.getNumChildren(); i++ ){
            n_args.push_back( n[i] );
          }
          for( int i=0; i<(int)d_single_inv_arg_sk.size(); i++ ){
            if( std::find( n_args.begin(), n_args.end(), d_single_inv_arg_sk[i] )==n_args.end() ){
              Trace("cegqi-si") << "...property is not ground: program invocation " << n << " does not contain variable " << d_single_inv_arg_sk[i] << "." << std::endl;
              //try to do variable elimination on d_single_inv_arg_sk[i]
              if( doVariableElimination( d_single_inv_arg_sk[i], conjuncts ) ){
                Trace("cegqi-si") << "...did variable elimination on " << d_single_inv_arg_sk[i] << std::endl;
                d_single_inv_arg_sk.erase( d_single_inv_arg_sk.begin() + i, d_single_inv_arg_sk.begin() + i + 1 );
                i--;
              }else{
                singleInvocation = false;
                //exit( 57 );
              }
              break;
            }
          }
        }

        if( singleInvocation ){
          Node bd = conjuncts.size()==1 ? conjuncts[0] : NodeManager::currentNM()->mkNode( OR, conjuncts );
          d_single_inv = NodeManager::currentNM()->mkNode( FORALL, pbvl, bd );
          Trace("cegqi-si-debug") << "...formula is : " << d_single_inv << std::endl;
          if( options::eMatching.wasSetByUser() ){
            Node bd = d_qe->getTermDatabase()->getInstConstantBody( d_single_inv );
            std::vector< Node > patTerms;
            std::vector< Node > exclude;
            inst::Trigger::collectPatTerms( d_qe, d_single_inv, bd, patTerms, inst::Trigger::TS_ALL, exclude );
            if( !patTerms.empty() ){
              Trace("cegqi-si-em") << "Triggers : " << std::endl;
              for( unsigned i=0; i<patTerms.size(); i++ ){
                Trace("cegqi-si-em") << "   " << patTerms[i] << std::endl;
              }
            }
          }
        }
      }
    }
  }
  if( !singleInvocation ){
    Trace("cegqi-si") << "Property is not single invocation." << std::endl;
    if( options::cegqiSingleInvAbort() ){
      Message() << "Property is not single invocation." << std::endl;
      exit( 0 );
    }
  }
}

bool CegConjectureSingleInv::doVariableElimination( Node v, std::vector< Node >& conjuncts ) {
  //all conjuncts containing v must contain a literal v != s for some s
  // if so, do DER on all such conjuncts
  TNode s;
  for( unsigned i=0; i<conjuncts.size(); i++ ){
    int status = 0;
    if( getVariableEliminationTerm( true, true, v, conjuncts[i], s, status ) ){
      Trace("cegqi-si-debug") << "Substitute " << s << " for " << v << " in " << conjuncts[i] << std::endl;
      Assert( !s.isNull() );
      conjuncts[i] = conjuncts[i].substitute( v, s );
    }else{
      if( status==1 ){
        Trace("cegqi-si-debug") << "Conjunct " << conjuncts[i] << " contains " << v << " but not in disequality." << std::endl;
        return false;
      }else{
        Trace("cegqi-si-debug") << "Conjunct does not contain " << v << "." << std::endl;
      }
    }
  }
  return true;
}

bool CegConjectureSingleInv::getVariableEliminationTerm( bool pol, bool hasPol, Node v, Node n, TNode& s, int& status ) {
  if( hasPol ){
    if( n.getKind()==NOT ){
      return getVariableEliminationTerm( !pol, true, v, n[0], s, status );
    }else if( pol && n.getKind()==EQUAL ){
      for( unsigned r=0; r<2; r++ ){
        if( n[r]==v ){
          status = 1;
          Node ss = n[r==0 ? 1 : 0];
          if( s.isNull() ){
            s = ss;
          }
          return ss==s;
        }
      }
      //did not match, now see if it contains v
    }else if( ( n.getKind()==OR && !pol ) || ( n.getKind()==AND && pol ) ){
      for( unsigned i=0; i<n.getNumChildren(); i++ ){
        if( getVariableEliminationTerm( pol, true, v, n[i], s, status ) ){
          return true;
        }
      }
      return false;
    }
  }
  if( n==v ){
    status = 1;
  }else{
    for( unsigned i=0; i<n.getNumChildren(); i++ ){
      getVariableEliminationTerm( pol, false, v, n[i], s, status );
    }
  }
  return false;
}

bool CegConjectureSingleInv::analyzeSygusConjunct( Node p, Node n, std::map< Node, std::vector< Node > >& children,
                                                            std::map< Node, std::map< Node, std::vector< Node > > >& prog_invoke,
                                                            std::vector< Node >& progs, std::map< Node, std::map< Node, bool > >& contains, bool pol ) {
  if( ( pol && n.getKind()==OR ) || ( !pol && n.getKind()==AND ) ){
    for( unsigned i=0; i<n.getNumChildren(); i++ ){
      if( !analyzeSygusConjunct( p, n[i], children, prog_invoke, progs, contains, pol ) ){
        return false;
      }
    }
  }else if( pol && n.getKind()==NOT && n[0].getKind()==FORALL ){
    if( !p.isNull() ){
      //do not allow nested quantifiers
      return false;
    }
    analyzeSygusConjunct( n[0][0], n[0][1], children, prog_invoke, progs, contains, false );
  }else{
    if( pol ){
      n = TermDb::simpleNegate( n );
    }
    Trace("cegqi-si") << "Sygus conjunct : " << n << std::endl;
    children[p].push_back( n );
    for( unsigned i=0; i<progs.size(); i++ ){
      prog_invoke[n][progs[i]].clear();
    }
    bool success = analyzeSygusTerm( n, prog_invoke[n], contains[n] );
    for( unsigned i=0; i<progs.size(); i++ ){
      std::map< Node, std::vector< Node > >::iterator it = prog_invoke[n].find( progs[i] );
      Trace("cegqi-si") << "  Program " << progs[i] << " is invoked " << it->second.size() << " times " << std::endl;
      for( unsigned j=0; j<it->second.size(); j++ ){
        Trace("cegqi-si") << "    " << it->second[j] << std::endl;
      }
    }
    return success;
  }
  return true;
}

bool CegConjectureSingleInv::analyzeSygusTerm( Node n, std::map< Node, std::vector< Node > >& prog_invoke, std::map< Node, bool >& contains ) {
  if( n.getNumChildren()>0 ){
    if( n.getKind()==FORALL ){
      //do not allow nested quantifiers
      return false;
    }
    //look at first argument in evaluator
    Node p = n[0];
    std::map< Node, std::vector< Node > >::iterator it = prog_invoke.find( p );
    if( it!=prog_invoke.end() ){
      if( std::find( it->second.begin(), it->second.end(), n )==it->second.end() ){
        it->second.push_back( n );
      }
    }
    for( unsigned i=0; i<n.getNumChildren(); i++ ){
      if( !analyzeSygusTerm( n[i], prog_invoke, contains ) ){
        return false;
      }
    }
  }else{
    //record this conjunct contains n
    contains[n] = true;
  }
  return true;
}



void CegConjectureSingleInv::computeProgVars( Node n ){
  if( d_prog_var.find( n )==d_prog_var.end() ){
    d_prog_var[n].clear();
    if( std::find( d_single_inv_sk.begin(), d_single_inv_sk.end(), n )!=d_single_inv_sk.end() ){
      d_prog_var[n][n] = true;
    }else if( n.getKind()==SKOLEM && std::find( d_single_inv_arg_sk.begin(), d_single_inv_arg_sk.end(), n )==d_single_inv_arg_sk.end() ){
      d_inelig[n] = true;
      return;
    }
    for( unsigned i=0; i<n.getNumChildren(); i++ ){
      computeProgVars( n[i] );
      if( d_inelig.find( n[i] )!=d_inelig.end() ){
        d_inelig[n] = true;
        return;
      }
      for( std::map< Node, bool >::iterator it = d_prog_var[n[i]].begin(); it != d_prog_var[n[i]].end(); ++it ){
        d_prog_var[n][it->first] = true;
      }
    }
  }
}

bool CegConjectureSingleInv::addInstantiation( std::vector< Node >& subs, std::vector< Node >& vars, 
                                               std::vector< Node >& coeff, std::vector< Node >& has_coeff, unsigned i, std::vector< Node >& lems ){
  if( i==d_single_inv_sk.size() ){
    for( unsigned j=0; j<has_coeff.size(); j++ ){
      //unsigned index = std::find( vars.begin(), vars.end(), has_coeff[j] )-vars.begin();
      //Assert( !coeff[index].isNull() );
      //must normalize TODO
      return false;
    }
    
    //check if we have already added this instantiation
    bool alreadyExists;
    if( options::incrementalSolving() ){
      alreadyExists = !d_c_inst_match_trie->addInstMatch( d_qe, d_single_inv, subs, d_qe->getUserContext() );
    }else{
      alreadyExists = !d_inst_match_trie.addInstMatch( d_qe, d_single_inv, subs );
    }
    if( alreadyExists ){
      return false;
    }else{
      Trace("cegqi-engine") << "  * single invocation: " << std::endl;
      for( unsigned j=0; j<vars.size(); j++ ){
        Node v = d_single_inv_map_to_prog[d_single_inv[0][j]];
        Trace("cegqi-engine") << "    * " << v;
        Trace("cegqi-engine") << " (" << vars[j] << ")";
        Trace("cegqi-engine") << " -> " << subs[j] << std::endl;
      }
      Node lem = d_single_inv[1].substitute( d_single_inv_var.begin(), d_single_inv_var.end(), subs.begin(), subs.end() );
      lem = Rewriter::rewrite( lem );
      Trace("cegqi-si") << "Single invocation lemma : " << lem << std::endl;
      if( std::find( d_lemmas_produced.begin(), d_lemmas_produced.end(), lem )==d_lemmas_produced.end() ){
        lems.push_back( lem );
        d_lemmas_produced.push_back( lem );
        d_inst.push_back( std::vector< Node >() );
        d_inst.back().insert( d_inst.back().end(), subs.begin(), subs.end() );
      }
      return true;
    }
  }else{
    eq::EqualityEngine* ee = d_qe->getMasterEqualityEngine();
    std::map< Node, std::map< Node, bool > > subs_proc;
    Node v = d_single_inv_map_to_prog[d_single_inv[0][i]];
    Node pv = d_single_inv_sk[i];
    Node pvr;
    Node pv_coeff;  //coefficient of pv in the equality we solve (null is 1)
    
    //[1] easy case : pv is in the equivalence class as another term not containing pv
    if( ee->hasTerm( pv ) ){
      pvr = ee->getRepresentative( pv );
      eq::EqClassIterator eqc_i = eq::EqClassIterator( pvr, ee );
      while( !eqc_i.isFinished() ){
        Node n = *eqc_i;
        if( n!=pv ){
          Trace("cegqi-si-inst-debug") << i << "...try based on equal term " << n << std::endl;
          //compute d_subs_fv, which program variables are contained in n
          computeProgVars( n );
          //must be an eligible term
          if( d_inelig.find( n )==d_inelig.end() ){
            Node ns;
            bool proc = false;
            if( !d_prog_var[n].empty() ){
              ns = applySubstitution( n, subs, vars, coeff, has_coeff, pv_coeff, false );
              if( !ns.isNull() ){
                computeProgVars( ns );
                //substituted version must be new and cannot contain pv
                proc = subs_proc[pv_coeff].find( ns )==subs_proc[pv_coeff].end() && d_prog_var[ns].find( pv )==d_prog_var[ns].end();
              }
            }else{
              ns = n;
              proc = true;
            }
            if( proc ){
              //try the substitution
              subs_proc[ns][pv_coeff] = true;
              if( addInstantiationInc( ns, pv, pv_coeff, subs, vars, coeff, has_coeff, i, lems ) ){
                return true;
              }
            }
          }
        }
        ++eqc_i;
      }
    }
    
    //[2] : we can solve an equality for pv
    ///iterate over equivalence classes to find cases where we can solve for the variable
    if( pv.getType().isInteger() || pv.getType().isReal() ){
      eq::EqClassesIterator eqcs_i = eq::EqClassesIterator( ee );
      while( !eqcs_i.isFinished() ){
        Node r = *eqcs_i;
        TypeNode rtn = r.getType();
        if( rtn.isInteger() || rtn.isReal() ){
          std::vector< Node > lhs;
          std::vector< bool > lhs_v;
          std::vector< Node > lhs_coeff;
          eq::EqClassIterator eqc_i = eq::EqClassIterator( r, ee );
          while( !eqc_i.isFinished() ){
            Node n = *eqc_i;
            computeProgVars( n );
            //must be an eligible term
            if( d_inelig.find( n )==d_inelig.end() ){
              Node ns;
              if( !d_prog_var[n].empty() ){
                ns = applySubstitution( n, subs, vars, coeff, has_coeff, pv_coeff );
                if( !ns.isNull() ){
                  computeProgVars( ns );
                }
              }else{
                ns = n;
                pv_coeff = Node::null();
              }
              if( !ns.isNull() ){
                bool hasVar = d_prog_var[ns].find( pv )!=d_prog_var[ns].end();
                for( unsigned j=0; j<lhs.size(); j++ ){
                  //if this term or the another has pv in it, try to solve for it
                  if( hasVar || lhs_v[j] ){
                    Trace("cegqi-si-inst-debug") << i << "...try based on equality " << lhs[j] << " " << ns << std::endl;
                    Node eq_lhs = lhs[j];
                    Node eq_rhs = ns;
                    //make the same coefficient
                    if( pv_coeff!=lhs_coeff[j] ){
                      if( !pv_coeff.isNull() ){
                        Trace("cegqi-si-inst-debug") << "...mult lhs by " << pv_coeff << std::endl;
                        eq_lhs = NodeManager::currentNM()->mkNode( MULT, pv_coeff, eq_lhs );
                        eq_lhs = Rewriter::rewrite( eq_lhs );
                      }
                      if( !lhs_coeff[j].isNull() ){
                        Trace("cegqi-si-inst-debug") << "...mult rhs by " << lhs_coeff[j] << std::endl;
                        eq_rhs = NodeManager::currentNM()->mkNode( MULT, lhs_coeff[j], eq_rhs );
                        eq_rhs = Rewriter::rewrite( eq_rhs );
                      }
                    }
                    Node eq = eq_lhs.eqNode( eq_rhs );
                    eq = Rewriter::rewrite( eq );
                    Trace("cegqi-si-inst-debug") << "...equality is " << eq << std::endl;
                    std::map< Node, Node > msum;
                    if( QuantArith::getMonomialSumLit( eq, msum ) ){
                      if( Trace.isOn("cegqi-si-inst-debug") ){
                        Trace("cegqi-si-inst-debug") << "...got monomial sum: " << std::endl;
                        QuantArith::debugPrintMonomialSum( msum, "cegqi-si-inst-debug" );
                        Trace("cegqi-si-inst-debug") << "isolate for " << pv << "..." << std::endl;
                      }
                      Node veq;
                      if( QuantArith::isolateEqCoeff( pv, msum, veq ) ){
                        Trace("cegqi-si-inst-debug") << "...isolated equality " << veq << "." << std::endl;
                        Node veq_c;
                        if( veq[0]!=v ){
                          Node veq_v;
                          if( QuantArith::getMonomial( veq[0], veq_c, veq_v ) ){
                            Assert( veq_v==v );
                          }
                        }
                        if( veq_c.isNull() ){ //TODO
                          computeProgVars( veq[1] );
                          subs_proc[veq[1]][veq_c] = true;
                          if( addInstantiationInc( veq[1], pv, veq_c, subs, vars, coeff, has_coeff, i, lems ) ){
                            return true;
                          }
                        }
                      }
                    }
                  }
                }
                lhs.push_back( ns );
                lhs_v.push_back( hasVar );
                lhs_coeff.push_back( pv_coeff );
              }
            }
            ++eqc_i;
          }
        }
        ++eqcs_i;
      }
    }
    
    //[3] directly look at assertions
    //TODO
    
    
    //[4] resort to using value in model     
    Node mv = d_qe->getModel()->getValue( pv );
    Trace("cegqi-si-inst-debug") << i << "...try model value " << mv << std::endl;
    return addInstantiationInc( mv, pv, pv_coeff, subs, vars, coeff, has_coeff, i, lems );
  }
}


bool CegConjectureSingleInv::addInstantiationInc( Node n, Node pv, Node pv_coeff, std::vector< Node >& subs, std::vector< Node >& vars, 
                                                  std::vector< Node >& coeff, std::vector< Node >& has_coeff, unsigned i, std::vector< Node >& lems ) {
  //substitute into previous substitutions, when applicable
  std::map< int, Node > prev;
  for( unsigned j=0; j<subs.size(); j++ ){
    Assert( d_prog_var.find( subs[j] )!=d_prog_var.end() );
    if( d_prog_var[subs[j]].find( pv )!=d_prog_var[subs[j]].end() ){
      prev[j] = subs[j];
      TNode tv = pv;
      TNode ts = n;
      subs[j] = subs[j].substitute( tv, ts );
      if( subs[j]!=prev[j] ){
        subs[j] = Rewriter::rewrite( subs[j] );
      }
    }
  }
  subs.push_back( n );
  vars.push_back( pv );
  coeff.push_back( pv_coeff );
  if( !pv_coeff.isNull() ){
    has_coeff.push_back( pv );
  }
  Trace("cegqi-si-inst-debug") << i << ": ";
  if( !pv_coeff.isNull() ){
    Trace("cegqi-si-inst-debug") << "*" << pv_coeff;
  }
  Trace("cegqi-si-inst-debug") << pv << " -> " << n << std::endl;
  if( addInstantiation( subs, vars, coeff, has_coeff, i+1, lems ) ){
    return true;
  }else{
    subs.pop_back();
    vars.pop_back();
    coeff.pop_back();
    if( !pv_coeff.isNull() ){
      has_coeff.pop_back();
    }
    //revert substitution
    for( std::map< int, Node >::iterator it = prev.begin(); it != prev.end(); it++ ){
      subs[it->first] = it->second;
    }
    return false;
  }
}

Node CegConjectureSingleInv::applySubstitution( Node n, std::vector< Node >& subs, std::vector< Node >& vars, 
                                                std::vector< Node >& coeff, std::vector< Node >& has_coeff, Node& pv_coeff, bool try_coeff ) {
  Assert( d_prog_var.find( n )!=d_prog_var.end() );
  bool req_coeff = false;
  if( !has_coeff.empty() ){
    for( std::map< Node, bool >::iterator it = d_prog_var[n].begin(); it != d_prog_var[n].end(); ++it ){
      if( std::find( has_coeff.begin(), has_coeff.end(), it->first )!=has_coeff.end() ){
        req_coeff = true;
        break;
      }
    }
  }
  if( !req_coeff ){
    Node nret = n.substitute( vars.begin(), vars.end(), subs.begin(), subs.end() );
    if( n!=nret ){
      Rewriter::rewrite( nret );
    }
    return nret;
  }else{
    if( try_coeff ){
      //must convert to monomial representation
      std::map< Node, Node > msum;
      if( QuantArith::getMonomialSum( n, msum ) ){
        //TODO
        
      }
    }
    // failed to apply the substitution
    return Node::null();
  }
}

void CegConjectureSingleInv::check( std::vector< Node >& lems ) {
  if( !d_single_inv.isNull() ) {
    std::vector< Node > subs;
    std::vector< Node > vars;
    std::vector< Node > coeff;
    std::vector< Node > has_coeff;
    //try to add an instantiation
    if( !addInstantiation( subs, vars, coeff, has_coeff, 0, lems ) ){
      Trace("cegqi-engine") << "  WARNING : unable to find CEGQI single invocation instantiation." << std::endl;
    }
  }
}

/*
bool CegConjectureSingleInv::solveEquality( Node lhs, Node rhs, int v, std::map< Node, int >& subs_from_model, std::vector< Node >& subs ) {
  Trace("cegqi-si-debug") << "Can possibly set " << d_single_inv_sk[v] << " based on " << lhs << " = " << rhs << std::endl;
  subs_from_model.erase( d_single_inv_sk[v] );
  Node prev_subs_v = subs[v];
  subs[v] = d_single_inv_sk[v];
  Node eq = lhs.eqNode( rhs );
  std::map< Node, Node > msum;
  if( QuantArith::getMonomialSumLit( eq, msum ) ){
    Trace("cegqi-si-debug") << "As monomial sum : " << std::endl;
    QuantArith::debugPrintMonomialSum( msum, "cegqi-si");
    Node veq;
    if( QuantArith::isolate( d_single_inv_sk[v], msum, veq, EQUAL ) ){
      Trace("cegqi-si-debug") << "Isolated for " << d_single_inv_sk[v] << " : " << veq << std::endl;
      Node sol;
      for( unsigned r=0; r<2; r++ ){
        if( veq[r]==d_single_inv_sk[v] ){
          sol = veq[ r==0 ? 1 : 0 ];
        }
      }
      if( !sol.isNull() ){
        sol = applyProgVarSubstitution( sol, subs_from_model, subs );
        Trace("cegqi-si-debug") << "Substituted solution is : " << sol << std::endl;
        subs[v] = sol;
        Trace("cegqi-engine") << "  ...by arithmetic, " << d_single_inv_sk[v] << " -> " << sol << std::endl;
        return true;
      }
    }
  }
  subs[v] = prev_subs_v;
  return false;
}
*/

Node CegConjectureSingleInv::constructSolution( unsigned i, unsigned index ) {
  Assert( index<d_inst.size() );
  Assert( i<d_inst[index].size() );
  if( index==d_inst.size()-1 ){
    return d_inst[index][i];
  }else{
    Node cond = d_lemmas_produced[index];
    cond = TermDb::simpleNegate( cond );
    Node ite1 = d_inst[index][i];
    Node ite2 = constructSolution( i, index+1 );
    return NodeManager::currentNM()->mkNode( ITE, cond, ite1, ite2 );
  }
}

Node CegConjectureSingleInv::getSolution( unsigned sol_index, TypeNode stn, int& reconstructed ){
  Assert( d_sol!=NULL );
  Assert( !d_lemmas_produced.empty() );
  const Datatype& dt = ((DatatypeType)(stn).toType()).getDatatype();
  Node varList = Node::fromExpr( dt.getSygusVarList() );
  Node prog = d_quant[0][sol_index];
  Node prog_app = d_single_inv_app_map[prog];
  //get variables
  std::vector< Node > vars;
  Trace("csi-sol") << "Get solution for " << prog << ", which is applied as " << prog_app << std::endl;
  Assert( prog_app.getNumChildren()==varList.getNumChildren()+1 );
  d_varList.clear();
  d_sol->d_varList.clear();
  for( unsigned i=1; i<prog_app.getNumChildren(); i++ ){
    if( varList[i-1].getType().isBoolean() ){
      //TODO force boolean term conversion mode
      Node c = NodeManager::currentNM()->mkConst(BitVector(1u, 1u));
      vars.push_back( prog_app[i].eqNode( c ) );
    }else{
      vars.push_back( prog_app[i] );
    }
    d_varList.push_back( varList[i-1] );
    d_sol->d_varList.push_back( varList[i-1] );
  }
  //construct the solution
  Node s = constructSolution( sol_index, 0 );
  s = s.substitute( vars.begin(), vars.end(), d_varList.begin(), d_varList.end() );
  d_orig_solution = s;

  //simplify the solution
  Trace("csi-sol") << "Solution (pre-simplification): " << d_orig_solution << std::endl;
  s = d_sol->simplifySolution( s, stn );
  Trace("csi-sol") << "Solution (post-simplification): " << s << std::endl;
  d_solution = s;

  //reconstruct the solution into sygus if necessary
  reconstructed = 0;
  if( options::cegqiSingleInvReconstruct() && !stn.isNull() ){
    d_sygus_solution = d_sol->reconstructSolution( s, stn, reconstructed );
    if( reconstructed==1 ){
      Trace("csi-sol") << "Solution (post-reconstruction into Sygus): " << d_sygus_solution << std::endl;
    }
  }


  if( Trace.isOn("csi-sol") ){
    //debug solution
    if( !d_sol->debugSolution( d_solution ) ){
      Trace("csi-sol") << "WARNING : solution " << d_solution << " contains free constants." << std::endl;
      //exit( 47 );
    }else{
      //exit( 49 );
    }
  }
  if( Trace.isOn("cegqi-stats") ){
    int tsize, itesize;
    tsize = 0;itesize = 0;
    d_sol->debugTermSize( d_orig_solution, tsize, itesize );
    Trace("cegqi-stats") << tsize << " " << itesize << " ";
    tsize = 0;itesize = 0;
    d_sol->debugTermSize( d_solution, tsize, itesize );
    Trace("cegqi-stats") << tsize << " " << itesize << " ";
    if( !d_sygus_solution.isNull() ){
      tsize = 0;itesize = 0;
      d_sol->debugTermSize( d_sygus_solution, tsize, itesize );
      Trace("cegqi-stats") << tsize << " - ";
    }else{
      Trace("cegqi-stats") << "null ";
    }
    Trace("cegqi-stats") << std::endl;
  }
  if( reconstructed==1 ){
    return d_sygus_solution;
  }else{
    return d_solution;
  }
}

}