import os
import ctypes
from loq2.types.state import State
from loq2.types.state import StatePointer,StateStruct

class ReturnType(ctypes.Structure):
    _fields_ = [
        ("action", ctypes.c_int),
        ("prev_state", StatePointer)
    ]

def Init(first):
    if(first):
        return [State(), -1]
    else:
        return [State().next(), -1]

def next(state, history):
    lib = ctypes.CDLL(os.getcwd()+"/Loq/main.so")
    lib.get.argtypes = [StatePointer, StatePointer, ctypes.c_int]
    lib.get.restype = ReturnType

    data = lib.get(state.obj, history[0].obj, history[1])
    prestate = State(data.prev_state)
    move = (data.action // 1000, (data.action % 1000) // 100,(data.action % 100) // 10, data.action % 10)
    return (move, (prestate, data.action))
