input = 'objcode.txt'

#addr = int("0801", 16)
addr = 16384

fp = open(input, 'r')
for line in fp:
    #print line
    codes = line.split()
    #print codes

    for code in codes:
        #hex = "0x"+code
        #print hex
        n = int(code, 16)
        #print code, n

        print "poke "+str(addr)+", "+str(n)
        addr += 1