#!/usr/bin/env python3
"""Fail safely if the protected Hall game key appears in tracked repository bytes.

The secret is read only from SPACEFORTRESS_HOF_API_KEY. Its value is never
printed, hashed, written, or included in an exception message.
"""

import os
import subprocess
import sys
from pathlib import Path


def tracked_files() -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", "-z"],
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    return [item.decode("utf-8") for item in result.stdout.split(b"\0") if item]


def main() -> int:
    secret = os.environ.get("SPACEFORTRESS_HOF_API_KEY", "")
    protected_names = {
        "Fab-HallOfFame-SpaceFortressVs-connexion.json",
    }
    tracked = tracked_files()

    bad_names = [path for path in tracked if Path(path).name in protected_names]
    if bad_names:
        for path in bad_names:
            print(f"ERROR: private Hall connection file is tracked: {path}", file=sys.stderr)
        return 2

    if not secret:
        print("Hall secret scan: build key not configured; tracked private-file guard passed.")
        return 0

    needle = secret.encode("utf-8")
    leaks: list[str] = []
    for path in tracked:
        try:
            data = Path(path).read_bytes()
        except OSError:
            continue
        if needle in data:
            leaks.append(path)

    if leaks:
        for path in leaks:
            print(f"ERROR: protected Hall game key found in tracked file: {path}", file=sys.stderr)
        return 3

    print("Hall secret scan: protected game key absent from tracked repository files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
