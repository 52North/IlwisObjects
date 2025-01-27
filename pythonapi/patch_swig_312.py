import sys

f = open(sys.argv[1])
lines = f.readlines()
i = 0
while i < len(lines):
	if lines[i].strip() == 'def swig_import_helper():':
		print(lines[i].rstrip('\n').rstrip('\r'))
		print('        path = os.path.dirname(__file__) + "/"')
		print('        os.environ["PATH"] = path + os.pathsep + os.environ["PATH"]')
		print('        os.add_dll_directory(path)')
		print('        sys.path.append(path)')
		print('        import _ilwisobjects')
		print('        return _ilwisobjects')
		i = i + 1
		while lines[i].strip() != '_ilwisobjects = swig_import_helper()':
			i = i + 1
		break
	else:
		print(lines[i].rstrip('\n').rstrip('\r'))
		i = i + 1
while i < len(lines):
	print(lines[i].rstrip('\n').rstrip('\r'))
	i = i + 1