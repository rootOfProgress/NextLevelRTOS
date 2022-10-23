with open("prog", "rb") as f:
    byte = f.read(1)
    while byte != b"":
        byte = f.read(1)
        print(byte)
