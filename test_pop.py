#!/usr/bin/python
import os
import time
i = 0
while i<500000:
	os.system('./redis-cli qpopn abc 3')
#time.sleep(1)
	i += 1
