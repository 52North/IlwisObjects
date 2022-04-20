import os
import sys

path = os.path.dirname(__file__)
os.environ["PATH"] = path + os.pathsep + os.environ["PATH"]
sys.path.append(path)
