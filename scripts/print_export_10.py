import glob

for f in glob.glob("unpacked/overlay/*.script.asm"):
    with open(f, 'r', encoding='utf-8', errors='ignore') as fp:
        lines = fp.readlines()
    
    # Let's find export#10
    start_idx = -1
    for i, line in enumerate(lines):
        if "fn export#10" in line:
            start_idx = i
            break
            
    if start_idx != -1:
        print(f"\n==================== File: {f} (export#10) ====================")
        # print lines until next export or end
        for line in lines[start_idx : start_idx + 30]:
            if "fn export#" in line and "fn export#10" not in line:
                break
            print(line.strip())
