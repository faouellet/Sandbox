import numpy as np

from jit import Jit


@Jit
def func():
    return 9*9


print func()
