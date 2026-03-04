from pypdf import PdfReader

pdf_path = r"c:\\Users\\regte\\Downloads\\PY32F002B Reference manual v1.0_EN.pdf"
r = PdfReader(pdf_path)

# Search for key phrases to confirm flash size
queries = [
    "0x0800 5FFF",
    "0x08005FFF",
    "24 Kbytes",
    "24K",
    "Flash memory is",
    "Flash size",
    "FLASHSIZE",
]

hits = {q: [] for q in queries}

for i in range(len(r.pages)):
    t = r.pages[i].extract_text() or ""
    if not t:
        continue
    for q in queries:
        if q.lower() in t.lower():
            hits[q].append(i+1)
    # Keep it bounded: if we already found the strongest hints, stop early
    if len(hits["Flash size"]) > 3 and len(hits["FLASHSIZE"]) > 3:
        break

for q in queries:
    if hits[q]:
        print(q, "->", hits[q][:20])
