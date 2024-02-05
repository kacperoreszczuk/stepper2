import os
import csv

files = [filename for filename in os.listdir() if filename.endswith(".csv")]

def parse_cpl(filename):
    raw = open(filename).read().strip()
    fields = [[word.strip().strip('"') for word in line.split(",")] for line in raw.split("\n")]
    ref, val, package, posx, posy, rot, side = list(map(list, zip(*fields)))
    ref[0] = "Designator"
    val[0] = "Val"
    package[0] = "Package"
    posx[0] = "Mid X"
    posy[0] = "Mid Y"
    rot[0] = "Rotation"
    side[0] = "Layer"
    output = "".join([",".join(line) + "\n" for line in zip(ref, val, package, posx, posy, rot, side)])
    open("CPL_" + filename, "w").write(output)


def parse_bom(filename):
    raw = open(filename).read().strip()
    fields = [[word.strip('"') for word in line.split(",")] for line in raw.split("\n")]
    print(fields)
    ref, val, footprint, _, _, lcsc, *_ = list(map(list, zip(*fields)))
    ref[0] = "Designator"
    val[0] = "Comment"
    footprint[0] = "Footprint"
    lcsc[0] = "LCSC"
    output = "".join([",".join(line) + "\n" for line in zip(val, ref, footprint, lcsc)])
    open("BOM_" + filename, "w").write(output)

for filename in files:
    type = None
    if filename.startswith("BOM") or filename.startswith("CPL"):
        continue
    if filename.endswith("-pos.csv"):
        parse_cpl(filename)
        continue
    parse_bom(filename) 
