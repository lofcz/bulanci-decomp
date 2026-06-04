# Round 9 — 65× `_Globals::FUN_*` workers

Manifest: [`agent_todos_65_globals_r9.json`](agent_todos_65_globals_r9.json)  
Protocol: [`ROUND9_GLOBALS_FUN_PROTOCOL.md`](ROUND9_GLOBALS_FUN_PROTOCOL.md)  
Results: [`agent_todos_65_globals_r9_results.jsonl`](agent_todos_65_globals_r9_results.jsonl)  
Reports: [`fun_recovery/r9_globals_task_NNN_report.md`](fun_recovery/)

| ID | Address | Band | Ghidra name | Prior hint |
|----|---------|------|-------------|------------|
| 001 | `0x00405280` | early | FUN_00405280 |  |
| 002 | `0x0043d5b0` | sim | FUN_0043D5B0 |  |
| 003 | `0x0043dae0` | sim | FUN_0043DAE0 |  |
| 004 | `0x0043e040` | sim | FUN_0043E040 |  |
| 005 | `0x0043e740` | sim | FUN_0043E740 |  |
| 006 | `0x004438a0` | sim | FUN_004438A0 |  |
| 007 | `0x00443db0` | sim | FUN_00443DB0 |  |
| 008 | `0x004442c0` | sim | FUN_004442C0 |  |
| 009 | `0x0044714a` | sim | FUN_0044714A |  |
| 010 | `0x004473e8` | sim | FUN_004473E8 |  |
| 011 | `0x0044751c` | sim | FUN_0044751C |  |
| 012 | `0x004476c1` | sim | FUN_004476C1 |  |
| 013 | `0x00447e6c` | sim | FUN_00447E6C |  |
| 014 | `0x004486a4` | sim | FUN_004486A4 |  |
| 015 | `0x00448765` | sim | FUN_00448765 |  |
| 016 | `0x00448a97` | sim | FUN_00448A97 | R6 — high-xref sim/other |
| 017 | `0x00449386` | sim | FUN_00449386 |  |
| 018 | `0x004494ec` | sim | FUN_004494EC |  |
| 019 | `0x00449bea` | sim | FUN_00449BEA |  |
| 020 | `0x0044a916` | sim | FUN_0044A916 |  |
| 021 | `0x0044a942` | sim | FUN_0044A942 |  |
| 022 | `0x0044ab44` | sim | FUN_0044AB44 |  |
| 023 | `0x0044ac8a` | sim | FUN_0044AC8A |  |
| 024 | `0x0044ae3a` | sim | FUN_0044AE3A |  |
| 025 | `0x0044ae46` | sim | FUN_0044AE46 |  |
| 026 | `0x0044b1cf` | sim | FUN_0044B1CF |  |
| 027 | `0x0044b490` | sim | FUN_0044B490 |  |
| 028 | `0x0044c1d9` | sim | FUN_0044C1D9 |  |
| 029 | `0x0044c1e2` | sim | FUN_0044C1E2 |  |
| 030 | `0x0044c3e1` | sim | FUN_0044C3E1 |  |
| 031 | `0x0044c482` | sim | FUN_0044C482 |  |
| 032 | `0x0044c895` | sim | FUN_0044C895 |  |
| 033 | `0x0044cc24` | sim | FUN_0044CC24 |  |
| 034 | `0x0044d594` | sim | FUN_0044D594 |  |
| 035 | `0x0044faab` | sim | FUN_0044FAAB |  |
| 036 | `0x0044fcdf` | sim | FUN_0044FCDF | R6 — sim cluster |
| 037 | `0x0044fe54` | sim | FUN_0044FE54 |  |
| 038 | `0x0044fe57` | sim | FUN_0044FE57 |  |
| 039 | `0x004500e2` | jpeg_codec | FUN_004500E2 |  |
| 040 | `0x0045011e` | jpeg_codec | FUN_0045011E |  |
| 041 | `0x00452869` | jpeg_codec | FUN_00452869 |  |
| 042 | `0x00452f0b` | jpeg_codec | FUN_00452F0B |  |
| 043 | `0x0045b480` | jpeg_codec | FUN_0045B480 | R7/R8 — libmad IMDCT half before III_imdct_l |
| 044 | `0x00460f30` | jpeg_codec | FUN_00460F30 | R7/R8 — jcprepct edge expand; caller prep vtable |
| 045 | `0x00461080` | jpeg_codec | FUN_00461080 | R7 — jcprepct row replicate |
| 046 | `0x00461160` | jpeg_codec | FUN_00461160 | R7 — jcprepct row shift |
| 047 | `0x00461460` | jpeg_codec | FUN_00461460 | R7 — decompress main buffer init |
| 048 | `0x00466920` | jpeg_codec | FUN_00466920 | R7/R8 — merged-upsampler dither table size |
| 049 | `0x00466a10` | jpeg_codec | FUN_00466A10 |  |
| 050 | `0x00466a30` | jpeg_codec | FUN_00466A30 | R7/R8 — zlib gen_codes idiv helper (PARTIAL) |
| 051 | `0x00466a50` | jpeg_codec | FUN_00466A50 | R8 — merged-upsampler colormap index tables (PARTI |
| 052 | `0x00466cc0` | jpeg_codec | FUN_00466CC0 |  |
| 053 | `0x00466d40` | jpeg_codec | FUN_00466D40 |  |
| 054 | `0x00466f00` | jpeg_codec | FUN_00466F00 |  |
| 055 | `0x00467020` | jpeg_codec | FUN_00467020 |  |
| 056 | `0x00467150` | jpeg_codec | FUN_00467150 |  |
| 057 | `0x00467300` | jpeg_codec | FUN_00467300 | R7/R8 — jdmerge row alloc helper |
| 058 | `0x00467340` | jpeg_codec | FUN_00467340 | R7 — caller of row alloc / upsampler vtable |
| 059 | `0x0046a840` | jpeg_codec | FUN_0046A840 | R6 — dispatch UNK |
| 060 | `0x0046b400` | jpeg_codec | FUN_0046B400 |  |
| 061 | `0x0046c690` | jpeg_codec | FUN_0046C690 |  |
| 062 | `0x0046c850` | jpeg_codec | FUN_0046C850 |  |
| 063 | `0x0046db50` | jpeg_codec | FUN_0046DB50 | R6 — dispatch UNK |
| 064 | `0x004713d0` | zlib_tail | FUN_004713D0 | R5 w09 — zlib FUN not audio |
| 065 | `0x00472420` | zlib_tail | FUN_00472420 | R5 w09 — zlib FUN not audio |
