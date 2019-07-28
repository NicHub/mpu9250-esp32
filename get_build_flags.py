#!/usr/bin/env python
# -*- coding: utf-8 -*-

import datetime

UTC_Now = datetime.datetime.now()
date = UTC_Now.strftime( '\\"%Y-%m-%d\\"' )
time = UTC_Now.strftime( '\\"%H:%M:%S\\"' )

flags = '''
    -D COMPILATION_DATE=%s
    -D COMPILATION_TIME=%s
''' % (date, time)

print(flags)
