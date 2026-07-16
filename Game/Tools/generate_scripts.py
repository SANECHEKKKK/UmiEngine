import os, re, sys

scripts_dir = sys.argv[1] if len(sys.argv) > 1 else "Scripts"
output_path = sys.argv[2] if len(sys.argv) > 2 else "Generated/RegisterScripts.cpp"

module_re = re.compile(r'export\s+module\s+([\w.]+)\s*;')
class_re  = re.compile(r'class\s+(\w+)\s*:\s*public\s+(?:Umi::)?BasicScript\b')

found = []  # (module_name, [class_names])
for dirpath, _, files in os.walk(scripts_dir):
    for fn in files:
        if not fn.endswith(".ixx"):
            continue
        with open(os.path.join(dirpath, fn), encoding="utf-8") as f:
            text = f.read()
        m = module_re.search(text)
        classes = class_re.findall(text)
        if m and classes:
            found.append((m.group(1), classes))

lines = ["// AUTO-GENERATED — do not edit.",
         "import ScriptManager;",
         "import Script;"]
lines += [f"import {mod};" for mod, _ in found]
lines += ["", "void RegisterAllScripts(Umi::ScriptManager& sm)", "{"]
for _, classes in found:
    for c in classes:
        lines.append(f'    sm.RegisterScript<Umi::{c}>("{c}");')
lines += ["}", "",
          'extern "C" __declspec(dllexport) void RegisterScripts(Umi::ScriptManager& sm)',
          "{", "    RegisterAllScripts(sm);", "}", ""]
content = "\n".join(lines)

old = open(output_path, encoding="utf-8").read() if os.path.exists(output_path) else None
if old != content:
    os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"Generated {output_path}")
else:
    print(f"{output_path} up to date")