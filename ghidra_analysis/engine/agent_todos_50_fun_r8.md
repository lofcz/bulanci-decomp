# Round 8 — 50× single `FUN_*` workers

Pool: **93** live `FUN_*` (post-R7). This round: **50**; **43** deferred.

Manifest: [`agent_todos_50_fun_r8.json`](agent_todos_50_fun_r8.json)  
Protocol: [`ROUND8_FUN_PROTOCOL.md`](ROUND8_FUN_PROTOCOL.md)  
Results: [`agent_todos_50_fun_r8_results.jsonl`](agent_todos_50_fun_r8_results.jsonl)  
Reports: [`fun_recovery/round8_fun_task_NN_report.md`](fun_recovery/)

| ID | Address | Xrefs | Band | Ghidra name | Prior hint |
|----|---------|------:|------|-------------|------------|
| 01 | `0x00466a30` | 2 | dispatch | FUN_00466A30 | R7 task 33 — zlib gen_codes idiv helper |
| 02 | `0x00467300` | 2 | dispatch | FUN_00467300 | R7 task 34 — jdmerge row alloc helper |
| 03 | `0x004370b0` | 1 | sim | FUN_004370B0 | R7 task 26/27 — track-switch iterator; caller SetCurrentTrac |
| 04 | `0x0045b480` | 1 | dispatch | FUN_0045B480 | R7 task 36 — libmad IMDCT half before III_imdct_l |
| 05 | `0x00460200` | 1 | dispatch | FUN_00460200 | R7 task 40 — jdmaster.c decompress master_selection split |
| 06 | `0x00460f30` | 1 | dispatch | FUN_00460F30 | R7 task 42 — jcprepct edge expand; caller prep vtable |
| 07 | `0x00461080` | 1 | dispatch | FUN_00461080 | R7 task 43 — jcprepct row replicate |
| 08 | `0x00461160` | 1 | dispatch | FUN_00461160 | R7 task 44 — jcprepct row shift |
| 09 | `0x004613e0` | 1 | dispatch | FUN_004613E0 | R7 task 42/43 — prep vtable start_pass homolog |
| 10 | `0x00461460` | 1 | dispatch | FUN_00461460 | R7 task 45 — decompress main buffer init |
| 11 | `0x00466920` | 1 | dispatch | FUN_00466920 | R7 task 50 — merged-upsampler dither table size |
| 12 | `0x00467340` | 1 | dispatch | FUN_00467340 | R7 task 34 — caller of row alloc |
| 13 | `0x00466a50` | 1 | dispatch | FUN_00466A50 | R7 task 50 — caller of dither size calc |
| 14 | `0x00448a97` | 5 | sim | FUN_00448A97 | R6 — high-xref sim/other |
| 15 | `0x00452fb7` | 4 | dispatch | FUN_00452FB7 | R6 — sim cluster |
| 16 | `0x0044fcdf` | 4 | sim | FUN_0044FCDF | R6 — sim cluster |
| 17 | `0x0046a840` | 3 | dispatch | FUN_0046A840 | R6 — dispatch UNK |
| 18 | `0x0046c3d0` | 3 | dispatch | FUN_0046C3D0 | R6 — dispatch UNK |
| 19 | `0x0046db50` | 2 | dispatch | FUN_0046DB50 | R6 — dispatch UNK |
| 20 | `0x00439710` | 2 | sim | FUN_00439710 | R6 — sim band UNK |
| 21 | `0x004466a0` | 2 | sim | FUN_004466A0 | R6 — sim UNK |
| 22 | `0x00449c90` | 2 | sim | FUN_00449C90 |  |
| 23 | `0x0044ab59` | 2 | sim | FUN_0044AB59 |  |
| 24 | `0x0044fe54` | 2 | sim | FUN_0044FE54 |  |
| 25 | `0x00472420` | 2 | eh | FUN_00472420 |  |
| 26 | `0x004500e2` | 1 | dispatch | FUN_004500E2 |  |
| 27 | `0x0045011e` | 1 | dispatch | FUN_0045011E |  |
| 28 | `0x00452869` | 1 | dispatch | FUN_00452869 |  |
| 29 | `0x00452f0b` | 1 | dispatch | FUN_00452F0B |  |
| 30 | `0x00454ace` | 1 | dispatch | FUN_00454ACE |  |
| 31 | `0x00466a10` | 1 | dispatch | FUN_00466A10 |  |
| 32 | `0x00466cc0` | 1 | dispatch | FUN_00466CC0 |  |
| 33 | `0x00466d40` | 1 | dispatch | FUN_00466D40 |  |
| 34 | `0x00466f00` | 1 | dispatch | FUN_00466F00 |  |
| 35 | `0x00467020` | 1 | dispatch | FUN_00467020 |  |
| 36 | `0x00467150` | 1 | dispatch | FUN_00467150 |  |
| 37 | `0x00467d10` | 1 | dispatch | FUN_00467D10 |  |
| 38 | `0x004683a0` | 1 | dispatch | FUN_004683A0 |  |
| 39 | `0x00468590` | 1 | dispatch | FUN_00468590 |  |
| 40 | `0x00468be0` | 1 | dispatch | FUN_00468BE0 |  |
| 41 | `0x00469060` | 1 | dispatch | FUN_00469060 |  |
| 42 | `0x004696a0` | 1 | dispatch | FUN_004696A0 |  |
| 43 | `0x00469900` | 1 | dispatch | FUN_00469900 |  |
| 44 | `0x0046a870` | 1 | dispatch | FUN_0046A870 |  |
| 45 | `0x0046abd0` | 1 | dispatch | FUN_0046ABD0 |  |
| 46 | `0x0046acf0` | 1 | dispatch | FUN_0046ACF0 |  |
| 47 | `0x0046adf0` | 1 | dispatch | FUN_0046ADF0 |  |
| 48 | `0x0046ae80` | 1 | dispatch | FUN_0046AE80 |  |
| 49 | `0x0046b000` | 1 | dispatch | FUN_0046B000 |  |
| 50 | `0x0046b0b0` | 1 | dispatch | FUN_0046B0B0 |  |
