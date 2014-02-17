import ast

from llvm import *
from llvm.core import *


class ASTVisitor(ast.NodeVisitor):
    def __init__(self, module):
        # Module in which the type will be defined
        self.module = module
        # State of the last node visited
        self.last_state = None

    def visit_ClassDef(self, node):
        pass

    def visit_Name(self, node):
        # Ignore names that aren't variables names such as decorators names
        if node.id in self.parameters:
            index = self.parameters.index(node.id)
            self.last_state = self.func_obj.args[index]
        return self.last_state

    def visit_Num(self, node):
        self.last_state = Constant.int(self.closure_type, node.n)
        return self.last_state

    def visit_BinOp(self, node):
        lhs = self.visit(node.left)
        rhs = self.visit(node.right)
        op = node.op

        if isinstance(op, ast.Sub):
            op = self.builder.sub(lhs, rhs, 'sub_t')
        elif isinstance(op, ast.Add):
            op = self.builder.add(lhs, rhs, 'add_t')
        elif isinstance(op, ast.Mult):
            op = self.builder.mul(lhs, rhs, 'mul_t')
        elif isinstance(op, ast.Div):
            op = self.builder.sdiv(lhs, rhs, 'div_t')
        elif isinstance(op, ast.Mod):
            op = self.builder.srem(lhs, rhs, 'rem_t')
        
        self.last_state = op
        return self.last_state
