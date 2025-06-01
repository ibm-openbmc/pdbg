#!/usr/bin/env python3

import subprocess
import os
from pathlib import Path

M4 = "m4"
DTC = "dtc"
M4_INCLUDE_DIR = "."
DTS_EXTS = ('.dts.m4', '.dtsi.m4')
dtb_files = []

# Skip special case .dts files handled separately
EXCLUDED_M4 = {'p9w-fsi.dts.m4', 'p9r-fsi.dts.m4', 'p9z-fsi.dts.m4', 'p8-pib.dts.m4'}
DTSI_M4_FILE = 'p9-fsi.dtsi.m4'
SRC_ROOT = Path(__file__).resolve().parent  
M4_DIR = SRC_ROOT / "m4"

def run_cmd(cmd, **kwargs):
    print(f"Running: {' '.join(cmd)}")
    subprocess.run(cmd, check=True, **kwargs)

def generate_dts_from_m4():
    m4_files = list(M4_DIR.rglob('*.dts.m4')) + list(M4_DIR.rglob('*.dtsi.m4'))
    generated = []

    for f in m4_files:
        if f.name in EXCLUDED_M4:
            continue

        out = f.with_suffix('')  # Remove .m4
        with out.open('w') as outfile:
            run_cmd([M4, '-I', str(f.parent), str(f)], stdout=outfile)
        generated.append(out)

    return generated

def generate_dtb_from_dts(dts_files):
    template = SRC_ROOT / 'template.S'
    if not template.exists():
        raise RuntimeError("Missing template.S for generating .S files")
    
    for dts in dts_files:
        if not dts.name.endswith('.dts'):
            continue  # Skip .dtsi or anything else
        dtb = dts.with_suffix('.dtb')
        run_cmd([DTC, '-I', 'dts', '-O', 'dtb', '-o', str(dtb), str(dts)])
        dtb_files.append(dtb)
        generate_s_file(dtb, template)
    
def generate_special_dts_files():
    for soc in ['p9w', 'p9r', 'p9z']:
        dts_m4 = M4_DIR / f'{soc}-fsi.dts.m4'
        dts = M4_DIR / f'{soc}-fsi.dts'
        run_cmd([M4, '-I', str(dts_m4.parent), str(dts_m4)], stdout=dts.open('w'))

def generate_p9_fsi_dtsi():
    """Generate p9-fsi.dtsi from p9-fsi.dtsi.m4"""
    src = M4_DIR / 'p9-fsi.dtsi.m4'
    dst = M4_DIR / 'p9-fsi.dtsi'

    if not src.exists():
        raise FileNotFoundError(f'Missing source file: {src}')

    print(f"Running: m4 -I {src.parent} {src.name}")
    run_cmd([M4, '-I', str(src.parent), str(src.name)], stdout=dst.open('w'))

def generate_dtb_with_p9_fsi():
    template = SRC_ROOT / 'template.S'
    """Generate DTBs from p9*-fsi.dts and p9-fsi.dtsi"""
    for soc in ['p9w', 'p9r', 'p9z']:
        dts = M4_DIR / f'{soc}-fsi.dts'
        dtb = M4_DIR / f'{soc}-fsi.dtb'
        run_cmd([
            'dtc', '-I', 'dts', '-O', 'dtb',
            '-i', str(dts.parent),
            '-o', str(dtb),
            str(dts)
        ])
        generate_s_file(dtb, template)
        dtb_files.append(dtb)

def generate_headers(dts_files):
    script = SRC_ROOT / 'generate_dt_header.sh'
    if not script.exists():
        raise RuntimeError("Missing script: generate_dt_header.sh")

    headers = []

    for dts in dts_files:
        if not dts.exists():
            continue

        if dts.suffix not in ['.dts', '.dtsi']:
            continue

        hname = dts.with_suffix('.dt.h')
        run_cmd(['sh', str(script), str(dts)], stdout=hname.open('w'))
        headers.append(hname)

    with (SRC_ROOT / 'generated_headers.txt').open('w') as f:
        for h in headers:
            f.write(str(h) + '\n')

def generate_s_file(dtb_name: str, template_name: str = 'template.S', cpp: str = 'cpp'):
    dtb_path = M4_DIR / dtb_name
    template_path = SRC_ROOT / template_name
    s_file = dtb_path.with_suffix(dtb_path.suffix + '.S')  # e.g., p9w-fsi.dtb.S

    if not template_path.exists():
        raise FileNotFoundError(f"Missing template: {template_path}")
    if not dtb_path.exists():
        raise FileNotFoundError(f"Missing dtb file: {dtb_path}")

    sym_prefix = dtb_path.name.replace('.', '_').replace('-', '_') + '_o'

    cmd = [
        cpp, str(template_path),
        f'-DSYMBOL_PREFIX={sym_prefix}',
        f'-DFILENAME="{dtb_path}"'
    ]

    print(f"Generating {s_file} from {dtb_path} using {template_path}")
    with open(s_file, 'w') as out:
        run_cmd(cmd, stdout=out)

def remove_all_existing_dts_dtb():
    for file_path in M4_DIR.rglob('*'):
        if file_path.suffix in ('.dts', '.dtb', '.S', '.h') and file_path.is_file():
            print(f"Removing {file_path}")
            file_path.unlink()


def main():
    print("==> Removing the earlier generated dtb and dts files...")
    remove_all_existing_dts_dtb()

    print("==> Generating DTS/DTSI from M4...")
    dts_files = generate_dts_from_m4()

    print("==> generate_p9_fsi_dtsi...")
    generate_p9_fsi_dtsi()

    print("==> generate_special_dts_files...")
    generate_special_dts_files()
    
    print("==> Generating DTBs for p9*-fsi with p9-fsi.dtsi included...")
    generate_dtb_with_p9_fsi()
    
    # Manually include the special DTS files
    special_dts = [M4_DIR / f'{soc}-fsi.dts' for soc in ['p9w', 'p9r', 'p9z']]
    all_dts = dts_files + special_dts

    print("==> Generating DT headers...")
    generate_headers(all_dts)

    print("==> Generating DTBs and .S file from regular DTS files...")
    generate_dtb_from_dts(dts_files)

        # Write generated dtb file paths to a file for Meson to read later
    with open('generated_dtb_files.txt', 'w') as f:
        for dtb in dtb_files:
            f.write(str(dtb.resolve()) + '\n')
    return dtb_files

    print("✅ Done!")

if __name__ == '__main__':
    main()
