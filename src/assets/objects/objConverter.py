file1 = open(r"teapot.obj", "r")
fileOverwrite = open("formattedPot.obj", "w")
fileOverwrite.write("")
fileOverwrite.close()
file2 = open("formattedPot.obj", "a")

while (x:= file1.readline()) != "":
    if x[0] != "f":
        file2.write(x)
    else:
        y = x.split(" ")
        file2.write(f"f {y[1]}/1/1 {y[2]}/1/1 {y[3].strip()}/1/1\n")
file1.close()
file2.close()
