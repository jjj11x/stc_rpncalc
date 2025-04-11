#!/usr/bin/env python3
"""\
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
"""

__author__ = "Ram Kromberg"
__license__ = "GPLv3+"
__version__ = "1.1"
__status__ = "Production"

import drawsvg as draw  # https://pypi.org/project/drawsvg/
import argparse

parser = argparse.ArgumentParser(description="Generate labels for https://github.com/jjj11x/stc_rpncalc", epilog="The generated scalable vector graphics files (.svg) were tested to print correctly using Firefox 137.0 under Linux while toggling 100% Scale rather than Fit to page width. Different systems use different fonts so it's recommended to convert to portable document format files (.pdf) or a raster format file for the purpose of redistribution.")
parser.add_argument("-n", "--negative", action='store_true', help="Toggles between the default positive, black-on-white output and and negative, white-on-black output.")
parser.add_argument("-b", "--bicolor", action='store_true', help="Toggles between the default, colorful output and black-and-white, bicolor output.")
args = parser.parse_args()
positive = not args.negative
bicolor = args.bicolor
spacing = 0
background = True
keyW, keyH = 1, 1  # to hardcode the key labels to 1cm wide and tall in the svg

labels = [
	[["Shift"], ["x ⇄ y", "⅟𝑥"], ["±", "√𝑥"], ["←", "CL𝑥"]],
	[["7", "yˣ"], ["8", "ln(𝑥)"], ["9", "log(𝑥)"], ["÷", "π"]],
	[["4", "R▼", "R▲"], ["5", "eˣ"], ["6", "10ˣ"], ["×"]],
	[["1", "sin(𝑥)", "asin(𝑥)"], ["2", "cos(𝑥)", "acos(𝑥)"], ["3", "tan(𝑥)", "atan(𝑥)"], ["-", "▶deg", "▶rad"]],
	[["0", "off"], [".", "STO"], ["Enter", "RCL"], ["+", "LAST𝑥"]]
]

fontSize = [
	[[14], [12,9], [26,9], [26,9]],
	[[14,9], [14,9], [14,9], [24,12]],
	[[14,9,9], [14,9], [14,9], [26]],
	[[14,9,9], [14,9,9], [14,9,9], [26,9,9]],
	[[14,9], [20,9], [12,9], [26,9]]
]

if positive:
	labelsFG = [
		[["darkblue"], ["black", "blue"], ["black", "blue"], ["black", "mediumblue"]],
		[["black", "mediumblue"], ["black", "mediumblue"], ["black", "mediumblue"], ["black", "blue"]],
		[["black", "mediumblue", "mediumblue"], ["black", "mediumblue"], ["black", "mediumblue"], ["black"]],
		[["black", "mediumblue", "mediumblue"], ["black", "mediumblue", "mediumblue"], ["black", "mediumblue", "mediumblue"], ["black", "blue", "blue"]],
		[["black", "mediumblue"], ["black", "mediumblue"], ["black", "blue"], ["black", "blue"]]
	]
	labelsBG = [
		["white", "white", "white", "tomato"],
		["lightgray", "lightgray", "lightgray", "white"],
		["lightgray", "lightgray", "lightgray", "white"],
		["lightgray", "lightgray", "lightgray", "white"],
		["lightgray", "lightgray", "white", "white"]
	]
	if bicolor:
		rectStroke = "black"
		midTextStroke = "black"
		upTextStroke = "black"
		downTextStroke ="black"
	else:
		rectStroke = "silver"
	background = "white"
else:
	labelsFG = [
		[["lightgray"], ["white", "lightgray"], ["white", "lightgray"], ["white", "lightgray"]],
		[["white", "lightgray"], ["white", "lightgray"], ["white", "lightgray"], ["white", "lightgray"]],
		[["white", "lightgray", "lightgray"], ["white", "lightgray"], ["white", "lightgray"], ["white"]],
		[["white", "lightgray", "lightgray"], ["white", "lightgray", "lightgray"], ["white", "lightgray", "lightgray"], ["white", "lightgray", "lightgray"]],
		[["white", "lightgray"], ["white", "lightgray"], ["white", "lightgray"], ["white", "lightgray"]]
	]
	labelsBG = [
		["black", "black", "black", "darkred"],
		["midnightblue", "midnightblue", "midnightblue", "black"],
		["midnightblue", "midnightblue", "midnightblue", "black"],
		["midnightblue", "midnightblue", "midnightblue", "black"],
		["midnightblue", "midnightblue", "black", "black"]
	]
	if bicolor:
		rectStroke = "white"
		midTextStroke = "white"
		upTextStroke = "white"
		downTextStroke ="white"
	else:
		rectStroke = "darkgray"
	background = "black"

columns, rows = len(labels[0]), len(labels)
imgW = (columns * keyW) + keyW + (spacing * (columns - 1))
imgH = (rows * keyH) + keyH + (spacing * (rows - 1))

d = draw.Drawing(str(imgW) + "cm", str(imgH) + "cm", origin=(0, 0))
if background:
	d.draw(draw.Rectangle(fill=background, x="0", y="0", width="100%", height="100%"))
for row in range(rows):
	for column in range(columns):
		rectX = (0.5 * keyW) + (column * keyW) + (spacing) * column
		rectY = (0.5 * keyH) + (row * keyH) + (spacing) * row
		rectWidth = keyW - (spacing / 2)
		rectHeight = keyH - (spacing / 2)
		label = labels[row][column]
		lenLabel = len(label)
		if bicolor:
			rectFill="none"
		else:
			rectFill=labelsBG[row][column]
			midTextStroke=labelsFG[row][column][0]
			if lenLabel > 1:
				upTextStroke=labelsFG[row][column][1]
			if lenLabel > 2:
				downTextStroke=labelsFG[row][column][2]
		d.draw(
			draw.Rectangle(
				fill=rectFill,
				stroke=rectStroke,
				stroke_width=0.5,
				x=str(rectX) + "cm",
				y=str(rectY) + "cm",
				width=str(rectWidth) + "cm",
				height=str(rectHeight) + "cm",
			)
		)
		d.append(
			draw.Text(
				label[0],
				font_size=fontSize[row][column][0],
				x=str(rectX + rectWidth / 2) + "cm",
				y=str(rectY + rectHeight / 2) + "cm",
				text_anchor="middle",
				dominant_baseline="central",
				fill=midTextStroke,
			)
		)
		if lenLabel > 1:
			d.append(
				draw.Text(
					label[1],
					font_size=fontSize[row][column][1],
					x=str(rectX + rectWidth / 2) + "cm",
					y=str(rectY + rectHeight / 2 - rectHeight / 3.5) + "cm",
					text_anchor="middle",
					dominant_baseline="central",
					fill=upTextStroke,
				)
			)
		if lenLabel > 2:
			d.append(
				draw.Text(
					label[2],
					font_size=fontSize[row][column][2],
					x=str(rectX + rectWidth / 2) + "cm",
					y=str(rectY + rectHeight / 2 + rectHeight / 3.5) + "cm",
					text_anchor="middle",
					dominant_baseline="central",
					fill=downTextStroke,
				)
			)

suffix = ""
if positive:
	suffix = suffix + "_positive"
else:
	suffix = suffix + "_negative"
if bicolor:
	suffix = suffix + "_bicolor"
d.save_svg("stc_rpncalc_labels"+suffix+".svg")
