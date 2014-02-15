import ast
import inspect


class ChplVisitor(ast.NodeVisitor):
    def __init__(self):
        self.chpl_code = ""

    def visit_Name(self, node):
        print 'NAME %s' % node.id

    def visit_Num(self, node):
        print 'NUM %s' % node.n

    def visit_BinOp(self, node):
        print 'BINOP %s' % node.id

    def visit_Assign(self, node):
        print 'ASSIGN %s' % node.targets[0].id
        self.visit(node.value)

    def visit_Call(self, node):
        print 'CALL %s' % node.func.attr
        [self.visit(arg) for arg in node.args] 

    def visit_List(self, node):
        print 'LIST'
        [self.visit(num) for num in node.elts]

    def visit_Return(self, node):
        print 'RETURN'


def dec(func):
    vis = ChplVisitor()
    tree = ast.parse(inspect.getsource(func))
    vis.visit(tree)


@dec
def test():
    x = np.array([1,2,3,4])
    y = np.array([5,6,7,8])
    z = np.dot(x, y)
    return z
