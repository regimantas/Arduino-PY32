from pypdf import PdfReader

pdf_path = r"c:\\Users\\regte\\Downloads\\PY32F002A Reference manual v1.0_EN.pdf"
r = PdfReader(pdf_path)

# Search a small window of pages for the explicit flash/SRAM size sentences
for page in range(10, 35):
    t = r.pages[page].extract_text() or ""
    tl = t.lower()
    if ("embedded sram" in tl) or ("features up to" in tl and "sram" in tl) or ("main flash" in tl) or ("flash memory consists" in tl):
        print("\npage", page+1)
        for ln in [ln.strip() for ln in t.splitlines() if ln.strip()]:
            l=ln.lower()
            if ('kbyte' in l) or ('sram' in l) or ('flash' in l) or ('0x0800' in l) or ('0x2000' in l):
                print(ln)
