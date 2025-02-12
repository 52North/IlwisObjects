from sysconfig import get_paths
from platform import python_version

info = get_paths()
include_path = info['include']
version = python_version()
version = '.'.join(version.split('.')[0:2])
f = open('pythonapi.pro')
lines = f.readlines()
f.close()
f = None
lines = [line.rstrip() for line in lines]
i = 0
f = open('pythonapi.pro', 'w')
while i < len(lines):
    line = lines[i]
    if line.strip() == 'PYMINORVERSION=8':
        f.write(line + '\n')
        i = i + 1
        line = lines[i]
        while line.strip() != 'OUTPUTPATHPYTHON=$$clean_path($$PWD/../output/$$PREFIX_COMPILER/$$CONF/ilwispy/ilwis)':
            i = i + 1
            line = lines[i]
        f.write('    PYTHON_INCLUDE_FILES=' + include_path + '\n')
        f.write('    LIBS += -L$$/usr/libs -lpython' + version + '\n')
    f.write(line + '\n')
    i = i + 1
f.flush()
f.close()
