def stringToFloats(str):
    "converts a string of space separated floats into a list of floats"
    ret = []
    lst = str.split()
    for i in lst:
        ret.append(float(i))
    return ret

def listOfTuples(filename):
    "A function to convert a space separated data file <filename> into a list of tuples"
    infile = open(filename)
    ret = []
    for line in infile:
        ret.append(tuple(stringToFloats(line)))
    return ret

