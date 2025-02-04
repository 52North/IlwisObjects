# Generate qt.props

import sys

qtdir=sys.argv[1]
pythonsearchpath=sys.argv[2]
pythonroots={'PYTHONROOT':pythonsearchpath+r'\Python36', 'PYTHONROOT37':pythonsearchpath+r'\Python37', 'PYTHONROOT38':pythonsearchpath+r'\Python38', 'PYTHONROOT39':pythonsearchpath+r'\Python39', 'PYTHONROOT310':pythonsearchpath+r'\Python310', 'PYTHONROOT311':pythonsearchpath+r'\Python311', 'PYTHONROOT312':pythonsearchpath+r'\Python312'}

print('<?xml version="1.0" encoding="utf-8"?>')
print('<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">')
print('  <ImportGroup Label="PropertySheets" />')
print('  <PropertyGroup Label="UserMacros">')
print('    <QTDIR>' + qtdir + '\</QTDIR>')
for root in pythonroots:
    print('    <' + root + '>' + pythonroots[root] + '</' + root + '>')
print('  </PropertyGroup>')
print('  <PropertyGroup />')
print('  <ItemDefinitionGroup />')
print('  <ItemGroup>')
print('    <BuildMacro Include="QTDIR">')
print('      <Value>$(QTDIR)</Value>')
print('      <EnvironmentVariable>true</EnvironmentVariable>')
print('    </BuildMacro>')
for root in pythonroots:
    print('    <BuildMacro Include="' + root + '">')
    print('      <Value>$(' + root + ')</Value>')
    print('    </BuildMacro>')
print('  </ItemGroup>')
print('</Project>')
