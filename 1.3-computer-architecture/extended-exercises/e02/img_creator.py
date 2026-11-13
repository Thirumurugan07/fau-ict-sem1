import sys, zlib, struct

def ppm_to_png(src, dst, scale=8):
    tokens = open(src).read().split()
    assert tokens[0] == "P3", "only P3 files"
    w, h = int(tokens[1]), int(tokens[2])
    vals = list(map(int, tokens[4:4 + 3 * w * h]))
    raw = bytearray()
    for y in range(h):
        row = bytearray()
        for x in range(w):
            i = 3 * (y * w + x)
            row += bytes(vals[i:i + 3]) * scale      # enlarge horizontally
        for _ in range(scale):                        # enlarge vertically
            raw += b"\x00" + row

    def chunk(kind, data):
        body = kind + data
        return (struct.pack(">I", len(data)) + body +
                struct.pack(">I", zlib.crc32(body) & 0xffffffff))

    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", w * scale, h * scale, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw)))
    png += chunk(b"IEND", b"")
    open(dst, "wb").write(png)

for f in sys.argv[1:]:
    ppm_to_png(f, f.rsplit(".", 1)[0] + ".png")