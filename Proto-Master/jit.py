import ast
import inspect

from llvm import *
from llvm.core import *
from llvm.ee import *
from llvm.passes import *

from llvm_visitor import ASTVisitor


class Jit():
    def __init__(self, func):
        self.func = func
        self.module = Module.new('jit_module')
        self.parse_tree = ast.parse(inspect.getsource(func))
        self.pm = PassManager.new()
    
    def __call__(self, *args, **kwargs):
        func_args_values = inspect.getcallargs(self.func, *args, **kwargs)
        self.visitor = ASTVisitor(self.module, func_args_values.keys())
        self.visitor.visit(self.parse_tree)
        self.visitor.build_return()

        # TODO: Test the -instcount pass
        self.pm.add(PASS_INSTCOUNT)
        print self.pm.run(self.module)
        
        self.ee = ExecutionEngine.new(self.module)
        
        args_values = [GenericValue.int(Type.int(), x) for x in func_args_values.values()]
         
        return self.ee.run_function(self.visitor.func_obj, args_values).as_int()

