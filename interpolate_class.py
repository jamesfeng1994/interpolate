import ctypes
import numpy as np
lib = ctypes.cdll.LoadLibrary('./interpolate.so')
c_float_p = ctypes.POINTER(ctypes.c_float)
class interpolate(object):
    def __init__(self, x, y, interpolate, extrapolate):
        lib.interpolate_new.argtypes = [c_float_p, ctypes.c_int, c_float_p, ctypes.c_int
                                       ,ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
        lib.interpolate_new.restype = ctypes.c_void_p
        lib.interpolate_interp.argtypes = [ctypes.c_void_p, ctypes.c_float]
        lib.interpolate_interp.restype = ctypes.c_float
        
        x = x.astype(np.float32)
        x_p = x.ctypes.data_as(c_float_p)
        y = y.astype(np.float32)
        y_p = y.ctypes.data_as(c_float_p)
        interpolate_p = ctypes.create_string_buffer(interpolate.encode('utf-8'))
        extrapolate_p = ctypes.create_string_buffer(extrapolate.encode('utf-8'))
        self.obj = lib.interpolate_new(x_p, len(x), y_p, len(y)
                                       ,interpolate_p, len(interpolate), extrapolate_p, len(extrapolate))
    def interp(self, xx):
        return lib.interpolate_interp(self.obj, xx)