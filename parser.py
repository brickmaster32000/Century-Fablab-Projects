d = open("C:\\Users\\voge0017\\Desktop\\LOG12_8_2012.txt","r")
a = open("C:\\Users\\voge0017\\Desktop\\axl.txt","w")
b = open("C:\\Users\\voge0017\\Desktop\\bar.txt","w")
lines = []

with d:
    data = d.readlines() #create a list from the input file, each index in list is one line from file
for i in range(len(data)):
    lines.append(data[i].rstrip('\n').split(","))

print lines

d.close()
a.close()
b.close()
print "I'm done."

