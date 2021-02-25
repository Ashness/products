#!/usr/bin/env python3
# -*- coding: utf-8 -*-

' a test module '

from hipnuc_module import *
import time
import os

res_file = 'res.txt'

if __name__ == '__main__':

    dev = hipnuc_module('./config.json')
    if os.path.exists(res_file):
      os.remove(res_file)
    f = open(res_file,'w')
    print ('write data to ' + res_file + '\n')
try:
    while True:
        data = dev.get_module_data()
        #print(data)
        f.write(str(data))
        f.write('\n')
        #time.sleep(0.10)
except KeyboardInterrupt:
    print("Press Ctrl-C to terminate while statement")
    dev.close()
    f.close()
    pass