#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
#=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        f.py
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-27 19:54:55
#  History:         
#=============================================================================
'''
import os

for i in range(0,600):
    print os.popen("./cli").read()
