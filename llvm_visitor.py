import ast

from llvm import *
from llvm.core import *


class ASTVisitor(ast.NodeVisitor):
    def __init__(self, module, parameters):
        self.last_state = None
        self.module = module
        self.parameters = parameters
        self.closure_type = Type.int()
        self.func_obj = None
        self._create_builder()

    def _create_builder(self):
        params = [self.closure_type] * len(self.parameters)

        ty_func = Type.function(self.closure_type, params)
        self.func_obj = self.module.add_function(ty_func, 'func_ast_jit')
        for index, pname in enumerate(self.parameters):
            self.func_obj.args[index].name = pname

        bb = self.func_obj.append_basic_block("entry")
        self.builder = Builder.new(bb)

    def visit_Name(self, node):
        # Ignore names that aren't variables' names such as decorators' names
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

        self.last_state = op
        return self.last_state

    def build_return(self):
        self.builder.ret(self.last_state)
