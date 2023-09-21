import re
from gradelib import *

needle = random_str()
dirs = [random_str() for _ in range(3)]
a = [
        'mkdir %s' % dirs[0],
        'echo > %s/%s' % (dirs[0], needle),
        'mkdir %s/%s' % (dirs[0], dirs[1]),
        'echo > %s/%s/%s' % (dirs[0], dirs[1], needle),
        'mkdir %s' % dirs[2],
        'echo > %s/%s' % (dirs[2], needle),
        'find . %s' % needle
    ]

for i in a:
    print(i)
