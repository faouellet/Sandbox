import ast
import inspect

import numpy as np

import pdb


class DotTransformer(ast.NodeTransformer):
    def __init__(self):
        self.node_to_add = None

    def visit_FunctionDef(self, node):
        new_body = []
        for child_node in node.body:
            new_body += [self.visit(child_node)]
            if self.node_to_add is not None:
                new_body += [self.node_to_add]
                self.node_to_add = None
        node.body = new_body
        return node

    def visit_Assign(self, node):
        new_node = node
        if hasattr(node.value, 'func'):
            if node.value.func.attr == 'dot':
                new_node.value = ast.Num(n=0)
                self.node_to_add = ast.For(
                        target=ast.Tuple(
                            elts=[ast.Name(id='i', ctx=ast.Store()), ast.Name(id='j', ctx=ast.Store())], ctx=ast.Store()), 
                        iter=ast.Tuple(
                            elts=[ast.Name(id='x', ctx=ast.Load()), ast.Name(id='y', ctx=ast.Load())], ctx=ast.Load()),
                        body=[ast.AugAssign(target=ast.Name(id='z', ctx=ast.Store()), op=ast.Add(), 
                            value=ast.BinOp(
                                left=ast.Name(id='x', ctx=ast.Load()),
                                op=ast.Mult(),
                                right=ast.Name(id='y', ctx=ast.Load())
                                ))
                            ],
                        orelse=[]
                        )
        return new_node
    
def dec(func):
    vis = DotTransformer()
    tree = ast.parse(inspect.getsource(func))
    new_tree = vis.visit(tree)
    print ast.dump(new_tree)


@dec
def test():
    x = np.array([1,2,3,4])
    y = np.array([5,6,7,8])
    z = np.dot(x, y)
    # z = 0
    # for i, j in x, y:
    #     z += i * j
    return z
    
