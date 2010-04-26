
from ez_setup import use_setuptools
use_setuptools()


from setuptools import setup, find_packages
from distutils.core import Extension
from Pyrex.Distutils import build_ext

setup(
  name = "chdb",
  version="1.0.0",
  author='Henk Punt',
  author_email='henkpunt@gmail.com',
  license = 'New BSD',
  description='',
  ext_modules=[
    Extension("_chdb", ["_chdb.pyx", "chdb.c"]),
    ],
  cmdclass = {'build_ext': build_ext},
    classifiers = [
             'Development Status :: 4 - Beta',
             'Environment :: Console',
             'Environment :: Web Environment',
             'Intended Audience :: Developers',
             'License :: OSI Approved :: BSD License',
             'Operating System :: OS Independent',
             'Programming Language :: Python',
             'Topic :: Software Development :: Libraries :: Python Modules'],
)

