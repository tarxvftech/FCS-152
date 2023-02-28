import os
import sys
import math
import cffi
import pprint
pp=pprint.pprint

path = os.getcwd()
ffi = cffi.FFI()
ffi.cdef("extern const unsigned char ascii_0408[66][4];")
ffi.cdef("extern const unsigned char pic_0408[3][4];")
ffi.cdef("extern const unsigned char ascii_0608[95][6];")
ffi.cdef("extern const unsigned char pic_0608[6][6];")
ffi.cdef("extern const unsigned char pic_0808[2][8];")
ffi.cdef("extern const unsigned char ascii_1016[96][20];")
# ffi.cdef("extern const unsigned char pic_0816;")
# ffi.cdef("extern const unsigned char pic_2424;")
# ffi.cdef("extern const unsigned char pic_1616;")
ffi.cdef("extern const unsigned char pic2516[10][50];")
ffi.cdef("extern const unsigned char pic_KDUClear[1024];")
ffi.cdef("extern const unsigned char pic_BaoTong[512];")
ffi.cdef("extern const unsigned char pic_HARRIS[512];")
ffi.set_source("_font",  # name of the output C extension
    """
    #include "font.h"
    """,
    sources=['font.cpp'],   # includes pi.c as additional sources
    libraries=[], #essentially a list of things that would be -l prefixed, like ['m'] -> -lm for math
    extra_compile_args=["-Wno-narrowing"]
    )

def show_pixels(cols,rows,bs):
    #each byte is a column of pixels
    bl = len(bs)
    sl = math.ceil(bl/cols)
    # print(cols, rows, bl, sl)
    assert(rows == sl*8)
    for row in range(sl):
        for bitpix in range(8):
            for col in range(cols):
                offset = cols*row+col
                b = (bs[offset] >> bitpix)&1
                if b:
                    sys.stdout.write("█")
                else:
                    sys.stdout.write(" ")
            sys.stdout.write("\n")
    sys.stdout.write("\n")

def read():
    ffi.compile(verbose=True)
    import _font
    globals().update({name: getattr(_font.lib, name) for name in dir(_font.lib)})
    print("picall")
    show_pixels(128,32,pic_HARRIS)
    # show_pixels(128,32,pic_BaoTong)
    # show_pixels(128,64,pic_KDUClear)
    # print("pics")
    # for i in range(len(pic_0408)):
        # show_pixels(4,8,pic_0408[i])
    # for i in range(len(pic_0608)):
        # show_pixels(6,8,pic_0608[i])
    # for i in range(len(pic_0808)):
        # show_pixels(8,8,pic_0808[i])
    # for i in range(len(pic2516)):
        # show_pixels(25,16,pic2516[i])

def bmp2list(imgpath):
    import struct
    with open(imgpath,"rb") as fd:
        bs = fd.read()
    filesize = struct.unpack_from('<L', bs, 2)[0]
    assert(filesize == len(bs))
    #ignore a few bytes
    first_pixel = struct.unpack_from('<L', bs, 0xA)[0]
    diblen = struct.unpack_from('<L', bs, 0xE)[0]
    width =  struct.unpack_from('<L', bs, 0x12)[0]
    height = struct.unpack_from('<L', bs, 0x16)[0]
    bpp =    struct.unpack_from('<H', bs, 0x1C)[0] #bits per pixel 
    fmt =    struct.unpack_from('<L', bs, 0x1E)[0]
    bitmap_data_sz = struct.unpack_from('<L', bs, 0x22)[0]
    assert(bpp == 24)
    assert(fmt == 0)
    Bpp = bpp/8
    pixels = []
    assert( width*height*Bpp == bitmap_data_sz ) #no padding with 24bit
    for y in range(height):
        row = []
        for x in range(width):
            offset = y*width*int(Bpp) + x*int(Bpp)
            b,g,r = struct.unpack_from('<BBB', bs, first_pixel + offset)
            assert(r in [255,0])
            assert(g in [255,0])
            assert(b in [255,0])
            row.append((r,g,b))
            if (width*Bpp)%4 != 0:
                raise(Exception("unhandled padding (which means something's wrong with your image file and it won't work on the FCS-152)")) #not handling it because our only supported format for the radio won't have it 
        pixels.append(row)
    assert(len(pixels) == height)
    assert(all(len(x) == width for x in pixels))
    return pixels[::-1] #invert so it's rows from top to bottom like usual (bmp files are bottom row first-in-file)

def drawbmplist2term(pixels):
    for y in pixels:
        for x in y:
            #\033[38;2;146;255;12mHello!\033[0m
            #https://stackoverflow.com/questions/74589665/how-to-print-rgb-colour-to-the-terminal
            r,g,b = x
            sys.stdout.write(f"\033[38;2;{r};{g};{b}m█\033[0m")
        sys.stdout.write('\n')
    sys.stdout.write('\n')

def bmplist2c_uncompress(pixels):
    height = len(pixels)
    width = len(pixels[0])
    print("const unsigned char whatever[%d][%d] = {"%(height,width))
    for y in pixels:
        sys.stdout.write('{\n')
        i = 0
        for x in y:
            sys.stdout.write('1,') #or whatever you want based on the value of x, which is an (r,g,b) tuple
            if i % 16 ==15:
                sys.stdout.write('\n')
            i+=1
        sys.stdout.write('},')
    print("}")

def bmplist2lcdbits(pixels):
    height = len(pixels)
    assert(height%8 == 0)
    width = len(pixels[0])
    bs = []
    numrows = int(height/8)
    for row in range(numrows):
        for x in range(width):
            b = 0
            # for i in range(7,-1,-1):
            for i in range(8):
                y = row*8 + i
                px = pixels[y][x]
                # print(row,y,x,i,px,all(px))
                b |= (all(px) << i)
            # print(b)
            bs.append(b)
    return bs

def bmp2lcdbits(imgpath):
    l = bmp2list(imgpath)
    return bmplist2lcdbits(l)

def pretty_c_array_hex(bs):
    s = "{\n"
    # s += ",".join("0x%02x"%(b) for b in bs)
    i = 0
    for b in bs:
        s += "0x%02x"%(b)
        if i != len(bs)-1:
            s += ', '
        if i%16 == 15:
            s+='\n'
        i+=1
    s += "};"
    return s

if __name__ == "__main__":
    read()
    drawbmplist2term(bmp2list("simple.bmp")) #sanity check bmp
    # drawbmplist2term(bmp2list("xvf152.bmp")) 
    c = bmp2lcdbits("xvf152.bmp")
    # print(c)
    show_pixels(128,32,c)
    print(pretty_c_array_hex(c))
