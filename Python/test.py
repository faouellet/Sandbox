import ast
import inspect
import pdb


def dec(some_class):
    print ast.dump(ast.parse(inspect.getsource(some_class)))
    return some_class


@dec
class Foo():
    def __init__(self):
        pass

    def bar(self):
        return 42


foo = Foo()
print foo.bar()
