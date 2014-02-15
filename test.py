import ast
import inspect
import numpy as np


def dec(func):
    print ast.dump(ast.parse(inspect.getsource(func)))
    return func 


@dec
def test():
    x = np.array([1,2,3,4])
    y = np.array([5,6,7,8])
    # z = np.dot(x, y)
    z = 0
    for i in range(len(x)):
        z += x[i]*y[i]
    return z


test()
