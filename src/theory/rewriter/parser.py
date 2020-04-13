import pyparsing as pp

from node import *
from rule import Rule

symbol_to_op = {
    'bvugt': Op.BVUGT,
    'bvuge': Op.BVUGE,
    'bvsgt': Op.BVSGT,
    'bvsge': Op.BVSGE,
    'bvslt': Op.BVSLT,
    'bvsle': Op.BVSLE,
    'bvult': Op.BVULT,
    'bvule': Op.BVULE,
    'bvneg': Op.BVNEG,
    'bvadd': Op.BVADD,
    'bvsub': Op.BVSUB,
    'bvshl': Op.BVSHL,
    'bvnot': Op.BVNOT,
    'concat': Op.CONCAT,
    'bv': Op.BVCONST,
    'zero_extend': Op.ZERO_EXTEND,
    'not': Op.NOT,
    '+': Op.PLUS,
    '-': Op.MINUS,
    '=': Op.EQ
}


def bv_to_int(s):
    assert s.startswith('bv')
    return int(s[2:])


def symbol():
    return pp.Word(pp.alphas + '=' + '_' + '+' + '-')

def mk_let(let):
    body = let[-1]
    for binding in reversed(let[0:-1]):
        body = Fn(Op.LET, [binding[0], binding[1], body])
    return body

def mk_case(cases):
    if not isinstance(cases[-1], Fn) or cases[-1].op != Op.CASE:
        cases[-1] = Fn(Op.CASE, [BoolConst(True), cases[-1]])
    return Fn(Op.COND, cases)


def expr():
    expr = pp.Forward()

    # Variable
    var = symbol().setParseAction(lambda s, l, t: Var(t[0]))

    # Constants
    bconst = pp.Keyword('true').setParseAction(
        lambda s, l, t: BoolConst(True)) | pp.Keyword('false').setParseAction(
            lambda s, l, t: BoolConst(False))
    iconst = pp.Word(
        pp.nums).setParseAction(lambda s, l, t: IntConst(int(t[0])))
    bvconst = (
        pp.Suppress('(') + pp.Suppress('_') + pp.Keyword('bv') + expr + expr +
        ')').setParseAction(lambda s, l, t: Fn(Op.BVCONST, [t[1], t[2]]))

    # Function applications
    indexed_app = (pp.Suppress('(') + pp.Suppress('(') + pp.Suppress('_') +
                   symbol() + pp.OneOrMore(expr) + pp.Suppress(')') +
                   pp.OneOrMore(expr) + pp.Suppress(')')).setParseAction(
                       lambda s, l, t: Fn(symbol_to_op[t[0]], t[1:]))
    app = (pp.Suppress('(') + symbol() + pp.OneOrMore(expr) + pp.Suppress(')')
           ).setParseAction(lambda s, l, t: Fn(symbol_to_op[t[0]], t[1:]))

    # Let bindings
    binding = (pp.Suppress('(') + var + expr + pp.Suppress(')')
               ).setParseAction(lambda s, l, t: (t[0], t[1]))
    let = (pp.Suppress('(') + pp.Keyword('let') + pp.Suppress('(') +
           pp.OneOrMore(binding) + pp.Suppress(')') + expr + pp.Suppress(')')
           ).setParseAction(lambda s, l, t: mk_let(t[1:]))

    # Conditionals
    case = (pp.Suppress('(') + expr + expr + pp.Suppress(')')
            ).setParseAction(lambda s, l, t: Fn(Op.CASE, [t[0], t[1]]))
    cond = (pp.Suppress('(') + pp.Keyword('cond') + pp.OneOrMore(case) +
            pp.Optional(expr) +
            pp.Suppress(')')).setParseAction(lambda s, l, t: mk_case(t[1:]))

    expr <<= bconst | iconst | bvconst | cond | indexed_app | app | let | var
    return expr


def sort():
    bv_sort = (pp.Suppress('(') + (pp.Suppress('_') + pp.Keyword('BitVec')) +
               expr() + pp.Suppress(')')
               ).setParseAction(lambda s, l, t: Sort(BaseSort.BitVec, [t[1]]))
    int_sort = pp.Keyword('Num').setParseAction(
        lambda s, l, t: Sort(BaseSort.Int, [], True))
    return bv_sort | int_sort


def var_decl():
    return (pp.Suppress('(') + pp.Word(pp.alphas) + sort() +
            pp.Suppress(')')).setParseAction(lambda s, l, t: (t[0], t[1]))


def var_list():
    return (pp.Suppress('(') + pp.OneOrMore(var_decl()) +
            pp.Suppress(')')).setParseAction(lambda s, l, t: dict(t[:]))


def parse_rules(s):
    comments = pp.ZeroOrMore(pp.Suppress(pp.cStyleComment))

    rule = comments + (pp.Suppress('(') + pp.Keyword('define-rule') +
                       pp.Word(pp.alphas) + var_list() + expr() + expr() +
                       pp.Suppress(')')).setParseAction(lambda s, l, t: Rule(
                           t[1], t[2], BoolConst(True), t[3], t[4]))
    rules = pp.OneOrMore(rule) + pp.StringEnd()
    return rules.parseString(s)
