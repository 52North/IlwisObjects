import os
from sysconfig import get_paths, get_config_var
import ctypes
from ctypes.util import find_library

class Dl_info(ctypes.Structure):
    _fields_ = [
        ("dli_fname", ctypes.c_char_p),
        ("dli_fbase", ctypes.c_void_p),
        ("dli_sname", ctypes.c_char_p),
        ("dli_saddr", ctypes.c_void_p),
    ]

info = get_paths()
include_path = info['include']

LDLIBRARY = get_config_var("LDLIBRARY")
if LDLIBRARY.endswith('.a'):
    LDLIBRARY = LDLIBRARY.rstrip('.a') + '.so'
libpython = ctypes.CDLL(LDLIBRARY)
libdl = ctypes.CDLL(find_library("dl"))
dlinfo = Dl_info()
retcode = libdl.dladdr(
    ctypes.cast(libpython.Py_GetVersion, ctypes.c_void_p),
    ctypes.pointer(dlinfo),
)
if retcode != 0:
    fullpath = os.path.realpath(dlinfo.dli_fname.decode())
    fullpath = fullpath.split('.so')[0]
    fullpath = fullpath.split('/')
    lib = '/'.join(fullpath[0:-1])
    name = fullpath[-1].lstrip('lib')
    f = open('pythonapi.pro')
    lines = f.readlines()
    f.close()
    f = None
    lines = [line.rstrip() for line in lines]
    i = 0
    f = open('pythonapi.pro', 'w')
    while i < len(lines):
        line = lines[i]
        if line.strip() == 'PYMINORVERSION=10':
            f.write(line + '\n')
            i = i + 1
            line = lines[i]
            while line.strip() != 'OUTPUTPATHPYTHON=$$clean_path($$PWD/../output/$$PREFIX_COMPILER/$$CONF/ilwispy/ilwis)':
                i = i + 1
                line = lines[i]
            f.write('    PYTHON_INCLUDE_FILES=' + include_path + '\n')
            f.write('    LIBS += -L' + lib + ' -l' + name + '\n')
        f.write(line + '\n')
        i = i + 1
    f.flush()
    f.close()
