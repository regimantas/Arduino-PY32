from pypdf import PdfReader
import re

pdf_path = r"c:\\Users\\regte\\Downloads\\PY32F002B Reference manual v1.0_EN.pdf"
reader = PdfReader(pdf_path)
print('pages', len(reader.pages))
addr_re = re.compile(r"0x[0-9A-Fa-f]{8}")

candidates = []
for i in range(min(80, len(reader.pages))):
    t = reader.pages[i].extract_text() or ''
    tl = t.lower()
    if 'memory map' in tl or ('memory' in tl and 'map' in tl):
        candidates.append(i)

print('memory-map candidates pages:', [i+1 for i in candidates][:20])

if not candidates:
    for i in range(min(150, len(reader.pages))):
        t = reader.pages[i].extract_text() or ''
        if any(a in t for a in ('0x08000000','0x20000000','0x40000000','0x50000000')):
            candidates.append(i)
    print('addr candidates pages:', [i+1 for i in candidates][:20])

for i in candidates[:3]:
    t = reader.pages[i].extract_text() or ''
    addrs = sorted(set(addr_re.findall(t)))
    print('\n--- page', i+1, 'addresses (first 80):')
    print(addrs[:80])
    lines = [ln.strip() for ln in t.splitlines() if ln.strip()]
    for ln in lines:
        if 'map' in ln.lower() or 'memory' in ln.lower() or '0x' in ln.lower():
            print(ln)
