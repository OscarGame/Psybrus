#!/usr/bin/python

import os
import shutil
import glob
import re

try:
	os.mkdir("assets")
	os.mkdir("assets/PackedContent")
except:
	pass

expr = re.compile(".*/PackedContent/(.*)")

files = glob.glob("../../../Dist/PackedContent/*")
for file in files:
	shutil.copy(file, "assets/PackedContent/" + expr.match(file).group(1) + ".mp3")