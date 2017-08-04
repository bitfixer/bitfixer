output = 'asmloaders.c'

func_name = 'load_image'
startaddr = 8192
size = 8000
loops = size / 256
rem = size - (loops * 256)
print loops, rem

f = open(output, 'w')

#init
f.write('void %s() {\n' % func_name)
f.write('asm("ldx #$00");\n')
f.write('asm("lda %w", 0xDD00);\n')
f.write('asm("tay");\n');

for i in range(0, loops):
    curr_addr = startaddr + (i*256)
    hex = '0x%04X' % curr_addr
    print i, curr_addr, hex

    #loop marker
    f.write('loop%d:\n' % i)
    #signal byte ready
    f.write('asm("tya");\n')
    f.write('asm("and #$FB");\n')
    f.write('asm("sta %w", 0xDD00);\n')
    #wait for remote
    f.write('wait%d:\n' % i)
    f.write('asm("lda %w", 0xDD0D);\n')
    f.write('asm("and #$10");\n')
    f.write('asm("beq %%g", wait%d);\n' % i)
    #load byte
    f.write('asm("lda %w", 0xDD01);\n')
    f.write('asm("sta %%w,x", %s);\n' % hex)
    #signal not ready
    f.write('asm("tya");\n')
    f.write('asm("ora #$04");\n')
    f.write('asm("sta %w", 0xDD00);\n')
    #wait for remote 2
    f.write('wait2_%d:\n' % i)
    f.write('asm("lda %w", 0xDD0D);\n')
    f.write('asm("and #$10");\n')
    f.write('asm("beq %%g", wait2_%d);\n' % i)
    #inc pointer
    f.write('asm("inx");\n')
    f.write('asm("bne %%g", loop%d);\n' % i)
    f.write('\n')

f.write('}\n')

f.close()

