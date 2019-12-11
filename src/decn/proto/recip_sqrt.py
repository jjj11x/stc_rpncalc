#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 11 00:56:01 2019

@author: jeffrey
"""

import numpy as np
import matplotlib.pyplot as plt

xvals = np.arange(1, 10, 0.01)
yvals = 10 ** (-0.5 * np.log10(xvals))
#linear fit for significand of 1/sqrt(x)
coef = np.polyfit(xvals, yvals, 1)
poly1d_fn = np.poly1d(coef)

plt.plot(xvals, yvals, 'b')
plt.plot(xvals, poly1d_fn(xvals), '--k')
plt.xlabel('significand')
plt.ylabel('1/sqrt(significand)')
plt.show()
print("linear approx coeff:", coef)
print("             * 3.16:", coef * 3.16227766)