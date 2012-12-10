
d = open("C:\\Users\\voge0017\\Desktop\\LOG12_8_2012.txt","r")

a = open("C:\\Users\\voge0017\\Desktop\\axl.txt","w")

b = open("C:\\Users\\voge0017\\Desktop\\bar.txt","w")

while True:
    text = d.readline()
    if 'Time' in text:
        a.write(text)
        b.write(text)
    if 'X1' in text:
        a.write(text)
    if 'X2' in text:
        a.write(text)
    if 'Y2' in text:
        a.write(text)
    if 'Z2' in text:
        a.write(text)
    if 'Pressure' in text:
        b.write(text)
    if 'Temperature' in text:
        b.write(text)
    if 'Altitude' in text:
        b.write(text)

d.close()
a.close()
b.close()
print "I'm done."
