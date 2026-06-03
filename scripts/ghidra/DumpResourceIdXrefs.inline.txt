// DumpResourceIdXrefs.java
//
// Inline Ghidra script (Java) that scans the open `bulanci.exe` program
// and writes one JSONL record per immediate-operand reference and per
// `.data`/`.rdata` u32 word whose value falls in the master-pack
// resource-ID range. Consumed by `scripts/build_asset_catalog.py`.
//
// Run via the Ghidra MCP server:
//
//     CallMcpTool user-ghidra-mcp run_script_inline \
//         --code "$(cat scripts/ghidra/DumpResourceIdXrefs.java)"
//
// or via Ghidra's Script Manager (drop the file into your local
// `ghidra_scripts/` directory and run it once).
//
// Output: ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl
//
// Three record shapes:
//
//   {"kind":"insn","addr":"00402b65","func":"CBulanci_OnCreate",
//    "funcAddr":"00402b20","mnemonic":"PUSH","insn":"PUSH 0x10004",
//    "opIdx":0,"id":65540,"idHex":"0x10004"}
//
//   {"kind":"data","addr":"004ae008","block":".data","id":65849,
//    "idHex":"0x10139","readers":[{"from":"00402b4f","func":"CBulanci_OnCreate",
//                                  "funcAddr":"00402b20","type":"DATA"}, ...]}
//
//   {"kind":"data-ptr","addr":"00482968","block":".rdata",
//    "from":"004201ac","func":"CGaming_ctor","funcAddr":"0041ff90",
//    "insn":"MOV [EBP-0x14], 0x482968"}
//
// `data-ptr` records describe instructions whose immediate operand
// points *into* `.rdata`/`.data` at an address that happens to be an
// ID-table head/interior. The catalog builder uses these to attribute
// table walkers (CWeapon::ctor, CGaming_ctor's prop-table loop, etc.)
// to every ID their pointed-at table spans -- without these the
// table-interior entries look orphaned because Ghidra's auto-xref
// only sees the table's BASE, not its sub-entries.
//
// IDs we care about live in [0x10000, 0x14000); the upper bound is
// generous to cover the few BitmapJpegAnim resources at 0x13xxx.
// `_is_crt_func` filtering and contiguous-table grouping happen in
// the Python catalog builder, not here — this script is intentionally
// "dumb but exhaustive".

import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import java.io.PrintWriter;
import java.io.FileWriter;

long ID_LO = 0x10000L;
long ID_HI = 0x14000L;
String outPath = "C:/Users/mstagl-dev/Documents/GitHub/bulanci/ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl";

Program p = getCurrentProgram();
Listing listing = p.getListing();
FunctionManager fm = p.getFunctionManager();
Memory mem = p.getMemory();
ReferenceManager refs = p.getReferenceManager();

PrintWriter w = new PrintWriter(new FileWriter(outPath));
int instrCount = 0;
int dataCount = 0;

java.util.function.Function<String,String> esc = (s) -> {
  if (s == null) return "";
  StringBuilder sb = new StringBuilder();
  for (int i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    if (c == '\\') sb.append("\\\\");
    else if (c == '"') sb.append("\\\"");
    else if (c == '\n') sb.append("\\n");
    else if (c == '\t') sb.append("\\t");
    else if (c < 0x20) sb.append(String.format("\\u%04x", (int) c));
    else sb.append(c);
  }
  return sb.toString();
};

// Pass 1: instructions with any scalar operand in [ID_LO, ID_HI)
InstructionIterator it = listing.getInstructions(true);
while (it.hasNext()) {
  Instruction insn = it.next();
  int nops = insn.getNumOperands();
  for (int i = 0; i < nops; i++) {
    Scalar s = insn.getScalar(i);
    if (s == null) continue;
    long v = s.getUnsignedValue();
    if (v < ID_LO || v >= ID_HI) continue;
    Function f = fm.getFunctionContaining(insn.getAddress());
    StringBuilder sb = new StringBuilder();
    sb.append("{\"kind\":\"insn\",");
    sb.append("\"addr\":\"").append(insn.getAddress().toString()).append("\",");
    sb.append("\"func\":\"").append(f != null ? esc.apply(f.getName()) : "").append("\",");
    sb.append("\"funcAddr\":\"").append(f != null ? f.getEntryPoint().toString() : "").append("\",");
    sb.append("\"mnemonic\":\"").append(esc.apply(insn.getMnemonicString())).append("\",");
    sb.append("\"insn\":\"").append(esc.apply(insn.toString())).append("\",");
    sb.append("\"opIdx\":").append(i).append(",");
    sb.append("\"id\":").append(v).append(",");
    sb.append("\"idHex\":\"0x").append(Long.toHexString(v)).append("\"}");
    w.println(sb.toString());
    instrCount++;
    break; // only the first matching scalar per insn
  }
}

// Pass 2: every initialised u32 in .data / .rdata that holds a resource ID, with readers
for (MemoryBlock blk : mem.getBlocks()) {
  if (!blk.isInitialized()) continue;
  String bname = blk.getName();
  if (!bname.equals(".data") && !bname.equals(".rdata")) continue;
  Address a = blk.getStart();
  Address end = blk.getEnd();
  while (a.compareTo(end) <= 0) {
    long v;
    try { v = mem.getInt(a) & 0xffffffffL; }
    catch (Exception e) { try { a = a.add(4); continue; } catch (Exception e2) { break; } }
    if (v >= ID_LO && v < ID_HI) {
      java.util.List<String> readers = new java.util.ArrayList<>();
      ReferenceIterator rit = refs.getReferencesTo(a);
      while (rit.hasNext()) {
        Reference r = rit.next();
        Address from = r.getFromAddress();
        Function f = fm.getFunctionContaining(from);
        String fname = f != null ? esc.apply(f.getName()) : "";
        String fEntry = f != null ? f.getEntryPoint().toString() : "";
        readers.add(String.format("{\"from\":\"%s\",\"func\":\"%s\",\"funcAddr\":\"%s\",\"type\":\"%s\"}",
            from.toString(), fname, fEntry, esc.apply(r.getReferenceType().toString())));
      }
      StringBuilder sb = new StringBuilder();
      sb.append("{\"kind\":\"data\",");
      sb.append("\"addr\":\"").append(a.toString()).append("\",");
      sb.append("\"block\":\"").append(bname).append("\",");
      sb.append("\"id\":").append(v).append(",");
      sb.append("\"idHex\":\"0x").append(Long.toHexString(v)).append("\",");
      sb.append("\"readers\":[").append(String.join(",", readers)).append("]}");
      w.println(sb.toString());
      dataCount++;
    }
    try { a = a.add(4); } catch (Exception e) { break; }
  }
}

// Pass 3: instructions whose immediate operand lands inside an
// initialised `.data`/`.rdata` block. These are the LEA / MOV /
// CMP / PUSH instructions that load a table-base or table-interior
// pointer (`[EDX*4 + 0x482820]`, `MOV ECX, 0x482968`, etc.). Used by
// the Python catalog builder to attribute every entry in a known
// ID-table to the function that walks it.
//
// We deliberately don't filter to "addresses we know hold IDs" here
// -- catalogs are easier to extend when the dumper is exhaustive and
// the consumer does the matching.
long DATA_RDATA_LO = -1L;
long DATA_RDATA_HI = -1L;
for (MemoryBlock blk : mem.getBlocks()) {
  String bname = blk.getName();
  if (!bname.equals(".data") && !bname.equals(".rdata")) continue;
  long s = blk.getStart().getOffset();
  long e = blk.getEnd().getOffset();
  if (DATA_RDATA_LO == -1L || s < DATA_RDATA_LO) DATA_RDATA_LO = s;
  if (DATA_RDATA_HI == -1L || e > DATA_RDATA_HI) DATA_RDATA_HI = e;
}

int dataPtrCount = 0;
java.util.function.Function<Long,String> findBlock = (off) -> {
  for (MemoryBlock blk : mem.getBlocks()) {
    String bname = blk.getName();
    if (!bname.equals(".data") && !bname.equals(".rdata")) continue;
    long s = blk.getStart().getOffset();
    long e = blk.getEnd().getOffset();
    if (off >= s && off <= e) return bname;
  }
  return null;
};

InstructionIterator it3 = listing.getInstructions(true);
while (it3.hasNext()) {
  Instruction insn = it3.next();
  // Skip instructions that already produced an `insn` record (i.e.
  // whose first scalar is an ID literal) so we don't double-dump.
  // We re-check each operand; reuse of Pass 1's logic for parity.
  int nops = insn.getNumOperands();
  for (int i = 0; i < nops; i++) {
    Scalar s = insn.getScalar(i);
    if (s == null) continue;
    long v = s.getUnsignedValue();
    // Limit to absolute addresses that fall inside .data/.rdata.
    if (v < DATA_RDATA_LO || v > DATA_RDATA_HI) continue;
    String bname = findBlock.apply(v);
    if (bname == null) continue;
    Function f = fm.getFunctionContaining(insn.getAddress());
    StringBuilder sb = new StringBuilder();
    sb.append("{\"kind\":\"data-ptr\",");
    sb.append("\"addr\":\"").append(String.format("%08x", v)).append("\",");
    sb.append("\"block\":\"").append(bname).append("\",");
    sb.append("\"from\":\"").append(insn.getAddress().toString()).append("\",");
    sb.append("\"func\":\"").append(f != null ? esc.apply(f.getName()) : "").append("\",");
    sb.append("\"funcAddr\":\"").append(f != null ? f.getEntryPoint().toString() : "").append("\",");
    sb.append("\"mnemonic\":\"").append(esc.apply(insn.getMnemonicString())).append("\",");
    sb.append("\"insn\":\"").append(esc.apply(insn.toString())).append("\",");
    sb.append("\"opIdx\":").append(i).append("}");
    w.println(sb.toString());
    dataPtrCount++;
    // Don't `break` -- some instructions (LEA EAX,[EDX*4 + 0x482820])
    // have multiple operands and we want each address.
  }
}

w.close();
println("wrote " + instrCount + " insn xrefs + " + dataCount + " data words + "
        + dataPtrCount + " data-ptr xrefs to " + outPath);
