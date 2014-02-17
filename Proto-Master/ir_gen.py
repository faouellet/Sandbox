import ast
import inspect

from llvm import *
from llvm.core import *
from llvm.ee import *
from llvm.passes import *

from llvm_visitor import ASTVisitor


class Jit():
    def __init__(self, some_class):
        self.original_class = some_class
        self.module = Module.new('jit_module')
        self.parse_tree = ast.parse(inspect.getsource(self.original_class))
        self.pm = PassManager.new() 
        self.visitor = ASTVisitor(self.module, func_args_values.keys())
        self.visitor.visit(self.parse_tree)
   
   def __call__(self, *args, **kwargs):
       # TODO: Identify function in module and execute it
       pass
