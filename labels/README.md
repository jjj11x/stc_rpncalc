# stc_rpncalc_labels

```
usage: stc_rpncalc_labels.py [-h] [-n] [-b]

Generate labels for https://github.com/jjj11x/stc_rpncalc

options:
  -h, --help      show this help message and exit
  -n, --negative  Toggles between the default positive, black-
                  on-white output and and negative, white-on-
                  black output.
  -b, --bicolor   Toggles between the default, colorful output
                  and black-and-white, bicolor output.

The generated scalable vector graphics files (.svg) were tested
to print correctly using Firefox 137.0 under Linux while
toggling 100% Scale rather than Fit to page width. Different
systems use different fonts so it's recommended to convert to
portable document format files (.pdf) or a raster format file
for the purpose of redistribution.
```

Depends on [drawsvg 2.4](https://pypi.org/project/drawsvg/) and was last tested on Python 3.12.
