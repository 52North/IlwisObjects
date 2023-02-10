#!/usr/bin/env bash
rm -r ../output/gccx64_/release/python_whl
mkdir ../output/gccx64_/release/python_whl
cp pythonapi/setup.py ../output/gccx64_/release/python_whl
cd ../output/gccx64_/release/python_whl
cp -r ../ilwispy/ilwis .
python3 ./setup.py bdist_wheel
rm -r build
rm -r ilwis
rm -r ilwis.egg-info
mv dist/*.whl .
rmdir dist
rm setup.py
# keep links instead of files (wish! not working!)
#unzip ilwispy-1.0-cp38-cp38-linux_x86_64.whl
#rm ilwispy-1.0-cp38-cp38-linux_x86_64.whl
#cp -r --preserve=mode,timestamps ../ilwispy/ilwis ilwispy-1.0.data/purelib/
#zip -ry ilwispy-1.0-cp38-cp38-linux_x86_64.whl ilwispy-1.0.data ilwispy-1.0.dist-info
#rm -r ilwispy-1.0.data
#rm -r ilwispy-1.0.dist-info
cd ../../../../IlwisObjects
