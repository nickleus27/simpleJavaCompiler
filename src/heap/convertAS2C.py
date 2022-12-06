file = open("alloc_fflist.s", 'r')
lines = file.readlines()
for line in lines:
    line = line.replace("\t", "\\t")
    line = line.replace("\n", "")
    c_line = "emit(\""+line + "\");"
    print(c_line)
