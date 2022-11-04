import os
import setuptools
import shutil 


def package_files(directory):
    paths = []
    for (path, directories, filenames) in os.walk(directory):
        for filename in filenames:
            paths.append(os.path.join('..', path, filename))
	
    return paths
basepath = '.\\ilwis'
extra_files = package_files(basepath)

def wheel_name(**kwargs):
    # create a fake distribution from arguments
    dist = Distribution(attrs=kwargs)
    # finalize bdist_wheel command
    bdist_wheel_cmd = dist.get_command_obj('bdist_wheel')
    bdist_wheel_cmd.ensure_finalized()
    # assemble wheel file name
    distname = bdist_wheel_cmd.wheel_dist_name
    tag = '-'.join(bdist_wheel_cmd.get_tag())
    return f'{distname}-{tag}.whl'


setup = setuptools.setup(
	name='ilwispy',
    version='1.0', 
  
    # Use one of the below approach to define package and/or module names:


	author="M.L.Schouwenburg",
    author_email="m.l.schouwenburg@utwente.nl",
    description="Extension giving access to the Ilwisobjects geo library for spatial data and processing",
    packages=setuptools.find_packages(include=['ilwis', 'ilwis.*']),
    classifiers=[
		'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Programming Language :: Python :: 3 :: Only',
        'Operating System :: Windows',
    ],
    python_requires='>=3.5',
	package_data={'': extra_files}
)
