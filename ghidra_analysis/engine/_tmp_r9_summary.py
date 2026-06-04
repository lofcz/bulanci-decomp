import json, glob, os, re
from collections import Counter

base = r"c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine"
reps = set()
for p in glob.glob(os.path.join(base, "fun_recovery", "r9_globals_task_*_report.md")):
    m = re.search(r"task_(\d+)", os.path.basename(p))
    if m:
        reps.add(int(m.group(1)))

jsonl_path = os.path.join(base, "agent_todos_65_globals_r9_results.jsonl")
lines = []
if os.path.isfile(jsonl_path):
    with open(jsonl_path) as f:
        for line in f:
            line = line.strip()
            if line:
                lines.append(json.loads(line))

by_id = {x["id"]: x for x in lines}
status = Counter(x.get("status") for x in lines)
done = [x for x in lines if x.get("status") == "DONE"]
missing_reports = sorted(set(range(1, 66)) - reps)
missing_jsonl = sorted(set(range(1, 66)) - set(by_id.keys()))

print(f"reports={len(reps)}/65 jsonl={len(lines)}/65")
print(f"status={dict(status)}")
print(f"missing_reports={missing_reports}")
print(f"missing_jsonl={missing_jsonl}")
print("DONE:")
for x in sorted(done, key=lambda z: z["id"]):
    print(f"  {x['id']:03d} {x.get('ghidra_name_after','?')}")
