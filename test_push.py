#!/usr/bin/python
import os
import time

i = 0
while i<500000:
    os.system('./redis-cli qpushn abc %d %d %d %s %d' % (i, i+1, i+2, i+3, i+4))
#time.sleep(1)
    i += 5
    print "index:", i
