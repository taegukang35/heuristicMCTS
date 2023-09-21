from loq2.types.state import StatePointer, StateStruct
from loq2.lib import lib
import ctypes as T

class ReturnType(T.Structure):
    _fields_ = [
        ("a", T.c_int),
        ("b", StateStruct),
    ]

lib.get_test.argtypes = [StatePointer, StatePointer, T.POINTER(T.c_int)]
lib.get_test.restype = ReturnType
a = State()
lib.get_test(a.obj, a.obj, T.pointer(T.c_int(0)))