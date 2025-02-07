import sys

nrParams = len(sys.argv)
solutionfile = sys.argv[-1]
projects = sys.argv[1:-1]
f = open(solutionfile)
lines = f.readlines()
lines = [line.rstrip() for line in lines]
ix = 0
while ix < len(lines):
    line = lines[ix]
    if line.startswith('Project'):
        skip = False
        for project in projects:
            if project in line:
                skip = True
                break
        if skip:
            while line != 'EndProject':
                ix = ix + 1
                line = lines[ix]
            ix = ix + 1
            line = lines[ix]
    print(line)
    ix = ix + 1
