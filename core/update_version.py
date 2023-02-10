from datetime import date

changed = False

d = date.today()
d = d.strftime("%Y%m%d")

versionfile = 'version.cpp'
f = open(versionfile, 'r')
lines = f.readlines()
f.close()
f = None
for i in range(len(lines)):
    line = lines[i]
    if 'ILWIS_VERSION_NUMBER' in line:
        ver = line.split()[-1].strip('"')
        if ver != d:
            line = line.split()
            line[-1] = d + '"'
            line = ' '.join(line)
            lines[i] = line + '\n'
            changed = True
        break

if changed:
    f = open(versionfile, 'w')
    f.writelines(lines)
    f.flush()
    f.close()
    f = None
