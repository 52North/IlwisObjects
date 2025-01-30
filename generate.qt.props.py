# Generate qt.props

qtdir=r'D:\Qt\5.12.1\msvc2017_64'
pythonroots={'PYTHONROOT':r'C:\Python36', 'PYTHONROOT37':r'C:\Python37', 'PYTHONROOT38':r'C:\Python38', 'PYTHONROOT39':r'C:\Python39', 'PYTHONROOT310':r'C:\Python310', 'PYTHONROOT311':r'C:\Python311', 'PYTHONROOT312':r'C:\Python312'}

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
