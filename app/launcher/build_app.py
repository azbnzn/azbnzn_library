#!/usr/bin/env python3
"""
Build script for AtCoder Apps.app (app/launcher/build_app.py)

Generates a crisp macOS .icns icon using pure stdlib + sips/iconutil, compiles
app/launcher/launcher_main.c with clang into a native Mach-O binary, packages
AtCoder Apps.app, signs it with ad-hoc codesign, installs copies to both
app/AtCoder Apps.app and ~/Applications/AtCoder Apps.app,
and registers them with LaunchServices so Spotlight/Dock pick them up immediately.
"""

import math
import os
import shutil
import struct
import subprocess
import sys
import tempfile
import zlib

LAUNCHER_DIR = os.path.dirname(os.path.abspath(__file__))
APP_DIR = os.path.abspath(os.path.join(LAUNCHER_DIR, ".."))
PORTAL_SCRIPT = os.path.join(APP_DIR, "portal.py")
LAUNCHER_SRC = os.path.join(LAUNCHER_DIR, "launcher_main.c")
APP_BUNDLE_NAME = "AtCoder Apps.app"

REPO_APP_DEST = os.path.join(APP_DIR, APP_BUNDLE_NAME)
USER_APPS_DIR = os.path.expanduser("~/Applications")
USER_APP_DEST = os.path.join(USER_APPS_DIR, APP_BUNDLE_NAME)

LSREGISTER = (
    "/System/Library/Frameworks/CoreServices.framework/Frameworks/"
    "LaunchServices.framework/Support/lsregister"
)


def detect_python_bin() -> str:
    """Resolve current Python 3 interpreter path cleanly."""
    candidate = os.path.realpath(sys.executable)
    if os.path.exists(candidate):
        return candidate
    return "/Library/Frameworks/Python.framework/Versions/3.13/bin/python3"


def make_png_bytes(width: int, height: int, rgba_rows: list[bytes]) -> bytes:
    """Encode RGBA pixel rows into a valid PNG image using pure stdlib zlib/struct."""
    def chunk(tag: bytes, data: bytes) -> bytes:
        crc = zlib.crc32(tag + data) & 0xFFFFFFFF
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", crc)

    raw = b"".join(b"\x00" + row for row in rgba_rows)
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    idat = zlib.compress(raw, level=9)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", idat) + chunk(b"IEND", b"")


def dist_point_to_segment(px: float, py: float, x1: float, y1: float, x2: float, y2: float) -> float:
    vx, vy = x2 - x1, y2 - y1
    wx, wy = px - x1, py - y1
    c1 = wx * vx + wy * vy
    if c1 <= 0.0:
        return math.hypot(px - x1, py - y1)
    c2 = vx * vx + vy * vy
    if c2 <= c1:
        return math.hypot(px - x2, py - y2)
    b = c1 / c2
    bx, by = x1 + b * vx, y1 + b * vy
    return math.hypot(px - bx, py - by)


def render_icon_png(out_path: str, size: int = 512) -> None:
    """Render an anti-aliased rounded-square dark blue/cyan AtCoder Apps icon."""
    s = float(size)
    half = s * 0.5
    box_half = s * 0.415
    radius = s * 0.19

    # Bold lightning bolt triangles inside center (normalized [-1, 1] -> pixel coords)
    def in_triangle(px: float, py: float, a, b, c) -> bool:
        d1 = (px - b[0]) * (a[1] - b[1]) - (a[0] - b[0]) * (py - b[1])
        d2 = (px - c[0]) * (b[1] - c[1]) - (b[0] - c[0]) * (py - c[1])
        d3 = (px - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (py - a[1])
        has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
        has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)
        return not (has_neg and has_pos)

    # Upper & lower bolt vertices in pixel coords
    t1_a = (s * 0.56, s * 0.21)
    t1_b = (s * 0.34, s * 0.54)
    t1_c = (s * 0.53, s * 0.52)

    t2_a = (s * 0.44, s * 0.79)
    t2_b = (s * 0.66, s * 0.46)
    t2_c = (s * 0.47, s * 0.48)

    bolt_edges = [
        (t1_a, t1_b), (t1_b, t1_c), (t1_c, t1_a),
        (t2_a, t2_b), (t2_b, t2_c), (t2_c, t2_a),
    ]

    # Small corner code brackets '<' and '>' subtle accents
    rows: list[bytes] = []
    for y in range(size):
        row = bytearray(size * 4)
        ny = y / s
        dy = max(abs(y + 0.5 - half) - (box_half - radius), 0.0)
        for x in range(size):
            dx = max(abs(x + 0.5 - half) - (box_half - radius), 0.0)
            sdf = math.hypot(dx, dy) - radius
            alpha_f = max(0.0, min(1.0, 1.0 - sdf))
            if alpha_f <= 0.0:
                continue

            nx = x / s
            # Deep navy -> sapphire diagonal gradient
            t = 0.55 * nx + 0.45 * ny
            r = 12.0 + 25.0 * t
            g = 22.0 + 48.0 * t
            b = 54.0 + 96.0 * t

            # Subtle radial cyan glow behind the bolt
            glow_d = math.hypot((x + 0.5 - half) / (s * 0.34), (y + 0.5 - half) / (s * 0.34))
            if glow_d < 1.0:
                gl = (1.0 - glow_d) ** 2 * 0.42
                r += (14.0 - r) * gl
                g += (165.0 - g) * gl
                b += (233.0 - b) * gl

            # Crisp cyan-blue inner rim stroke (~3px)
            if -5.0 < sdf < 0.0:
                rim = max(0.0, 1.0 - abs(sdf + 2.2) / 2.2) * 0.65
                r += (56.0 - r) * rim
                g += (189.0 - g) * rim
                b += (248.0 - b) * rim

            # Foreground bolt with smooth 1.5px AA edge
            inside_bolt = in_triangle(x + 0.5, y + 0.5, t1_a, t1_b, t1_c) or in_triangle(
                x + 0.5, y + 0.5, t2_a, t2_b, t2_c
            )
            min_edge = min(
                dist_point_to_segment(x + 0.5, y + 0.5, p1[0], p1[1], p2[0], p2[1])
                for p1, p2 in bolt_edges
            )
            if inside_bolt:
                bolt_cov = 1.0
            elif min_edge < 1.4:
                bolt_cov = max(0.0, 1.0 - min_edge / 1.4)
            else:
                bolt_cov = 0.0

            if bolt_cov > 0.0:
                # Warm golden -> bright cyan-white vertical gradient on bolt
                bt = max(0.0, min(1.0, (y - s * 0.2) / (s * 0.6)))
                br = 253.0 - 15.0 * bt
                bg = 235.0 - 40.0 * bt
                bb = 130.0 + 115.0 * (1.0 - bt) * 0.4
                r = r * (1.0 - bolt_cov) + br * bolt_cov
                g = g * (1.0 - bolt_cov) + bg * bolt_cov
                b = b * (1.0 - bolt_cov) + bb * bolt_cov

            idx = x * 4
            row[idx] = int(max(0, min(255, round(r))))
            row[idx + 1] = int(max(0, min(255, round(g))))
            row[idx + 2] = int(max(0, min(255, round(b))))
            row[idx + 3] = int(max(0, min(255, round(alpha_f * 255.0))))
        rows.append(bytes(row))

    png_data = make_png_bytes(size, size, rows)
    with open(out_path, "wb") as f:
        f.write(png_data)


def build_icns(work_dir: str, icns_out: str) -> None:
    """Create macOS AppIcon.icns from rendered master PNG via sips and iconutil."""
    master_png = os.path.join(work_dir, "master_512.png")
    render_icon_png(master_png, size=512)

    iconset_dir = os.path.join(work_dir, "AppIcon.iconset")
    os.makedirs(iconset_dir, exist_ok=True)

    icon_specs = [
        (16, "icon_16x16.png"),
        (32, "icon_16x16@2x.png"),
        (32, "icon_32x32.png"),
        (64, "icon_32x32@2x.png"),
        (128, "icon_128x128.png"),
        (256, "icon_128x128@2x.png"),
        (256, "icon_256x256.png"),
        (512, "icon_256x256@2x.png"),
        (512, "icon_512x512.png"),
        (1024, "icon_512x512@2x.png"),
    ]
    for px, filename in icon_specs:
        dest = os.path.join(iconset_dir, filename)
        subprocess.run(
            ["sips", "-z", str(px), str(px), master_png, "--out", dest],
            check=True,
            stdout=subprocess.DEVNULL,
        )

    subprocess.run(
        ["iconutil", "-c", "icns", iconset_dir, "-o", icns_out],
        check=True,
    )


def write_info_plist(plist_path: str) -> None:
    plist_xml = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>ja</string>
    <key>CFBundleDisplayName</key>
    <string>AtCoder Apps</string>
    <key>CFBundleExecutable</key>
    <string>AtCoderApps</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundleIdentifier</key>
    <string>com.azbnzn.atcoder-apps</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>AtCoder Apps</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>LSUIElement</key>
    <true/>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
"""
    with open(plist_path, "w", encoding="utf-8") as f:
        f.write(plist_xml)


def c_escape(val: str) -> str:
    return val.replace("\\", "\\\\").replace('"', '\\"')


def compile_launcher(binary_out: str, python_bin: str) -> None:
    cmd = [
        "clang",
        "-O2",
        "-Wall",
        "-Wextra",
        f'-DAPP_DIR="{c_escape(APP_DIR)}"',
        f'-DPORTAL_SCRIPT="{c_escape(PORTAL_SCRIPT)}"',
        f'-DPYTHON_BIN="{c_escape(python_bin)}"',
        LAUNCHER_SRC,
        "-o",
        binary_out,
    ]
    subprocess.run(cmd, check=True)
    os.chmod(binary_out, 0o755)


def install_bundle(src_bundle: str, dest_bundle: str) -> None:
    os.makedirs(os.path.dirname(dest_bundle), exist_ok=True)
    if os.path.exists(dest_bundle):
        shutil.rmtree(dest_bundle)
    subprocess.run(["ditto", src_bundle, dest_bundle], check=True)
    subprocess.run(["codesign", "--force", "--deep", "--sign", "-", dest_bundle], check=True)
    if os.path.exists(LSREGISTER):
        subprocess.run([LSREGISTER, "-f", dest_bundle], check=False)


def main():
    if not os.path.isfile(LAUNCHER_SRC):
        raise SystemExit(f"Missing launcher source: {LAUNCHER_SRC}")
    if not os.path.isfile(PORTAL_SCRIPT):
        raise SystemExit(f"Missing portal script: {PORTAL_SCRIPT}")

    python_bin = detect_python_bin()
    print(f"[build] Python binary : {python_bin}")
    print(f"[build] Portal script : {PORTAL_SCRIPT}")

    os.chmod(PORTAL_SCRIPT, 0o755)

    with tempfile.TemporaryDirectory(prefix="atcoder_apps_build_") as tmpdir:
        bundle_stage = os.path.join(tmpdir, APP_BUNDLE_NAME)
        macos_dir = os.path.join(bundle_stage, "Contents", "MacOS")
        res_dir = os.path.join(bundle_stage, "Contents", "Resources")
        os.makedirs(macos_dir, exist_ok=True)
        os.makedirs(res_dir, exist_ok=True)

        binary_path = os.path.join(macos_dir, "AtCoderApps")
        icns_path = os.path.join(res_dir, "AppIcon.icns")
        plist_path = os.path.join(bundle_stage, "Contents", "Info.plist")

        print("[build] Compiling native launcher (clang -O2)...")
        compile_launcher(binary_path, python_bin)

        print("[build] Generating AppIcon.icns...")
        build_icns(tmpdir, icns_path)

        print("[build] Writing Info.plist...")
        write_info_plist(plist_path)

        print(f"[build] Installing -> {REPO_APP_DEST}")
        install_bundle(bundle_stage, REPO_APP_DEST)

        print(f"[build] Installing -> {USER_APP_DEST}")
        install_bundle(bundle_stage, USER_APP_DEST)

    print("[build] Verifying code signatures...")
    for dest in (REPO_APP_DEST, USER_APP_DEST):
        subprocess.run(["codesign", "--verify", "--deep", "--strict", dest], check=True)
        print(f"[ok] Verified: {dest}")

    print("\n[done] AtCoder Apps.app のビルドとインストールが完了しました。")
    print("  - Spotlight (Cmd+Space -> 'AtCoder Apps') から起動できます。")
    print(f"  - Dock に追加したい場合は {USER_APP_DEST} を Dock にドラッグしてください。")


if __name__ == "__main__":
    main()
