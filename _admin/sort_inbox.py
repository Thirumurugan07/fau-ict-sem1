#!/usr/bin/env python3
"""Preview or apply keyword-based moves from _inbox/ into course folders."""

import argparse
import re
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
INBOX = ROOT / "_inbox"
COURSE_KEYWORDS = {
    "1.1-embedded-systems": ("ES", "Eingebettete"),
    "1.2-design-of-ics": ("DIC", "EIS"),
    "1.3-computer-architecture": ("RA", "Rechnerarchitektur"),
    "1.4-reconfigurable-computing": ("RC", "Reconfigurable"),
    "1.5-mlsp": ("MLSP",),
}
TYPE_KEYWORDS = {
    "slides": ("Vorlesung", "Lecture", "L0"),
    "exercises": ("Uebung", "Exercise", "Blatt", "Sheet"),
    "past-exams": ("Klausur", "Exam"),
}
ABBREVIATIONS = {"ES", "DIC", "EIS", "RA", "RC", "MLSP", "L0"}


def contains_keyword(filename: str, keyword: str) -> bool:
    if keyword.upper() in ABBREVIATIONS:
        return re.search(rf"(?<![A-Za-z0-9]){re.escape(keyword)}(?![A-Za-z0-9])", filename, re.I) is not None
    return keyword.casefold() in filename.casefold()


def match_destination(filename: str) -> Path | None:
    course_matches = [
        course
        for course, keywords in COURSE_KEYWORDS.items()
        if any(contains_keyword(filename, keyword) for keyword in keywords)
    ]
    type_matches = [
        category
        for category, keywords in TYPE_KEYWORDS.items()
        if any(contains_keyword(filename, keyword) for keyword in keywords)
    ]
    if len(course_matches) != 1 or len(type_matches) != 1:
        return None
    return ROOT / course_matches[0] / "sources" / type_matches[0]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--apply", action="store_true", help="move matched files (default: dry-run)")
    args = parser.parse_args()

    if not INBOX.is_dir():
        print(f"Error: inbox folder not found: {INBOX}")
        return 1

    items = sorted(INBOX.iterdir(), key=lambda path: path.name.casefold())
    if not items:
        print("_inbox/ is empty.")
        return 0

    unmatched = []
    for item in items:
        if item.name == ".gitkeep":
            continue
        if not item.is_file():
            unmatched.append(item.name)
            print(f"UNMATCHED (not a file): {item.name}")
            continue

        destination_dir = match_destination(item.name)
        if destination_dir is None:
            unmatched.append(item.name)
            print(f"UNMATCHED (course/type keywords not uniquely identified): {item.name}")
            continue

        destination = destination_dir / item.name
        if destination.exists():
            unmatched.append(item.name)
            print(f"UNMATCHED (destination exists): {item.name} -> {destination.relative_to(ROOT)}")
            continue

        print(f"{'MOVE' if args.apply else 'WOULD MOVE'}: {item.name} -> {destination.relative_to(ROOT)}")
        if args.apply:
            destination_dir.mkdir(parents=True, exist_ok=True)
            shutil.move(str(item), str(destination))

    if unmatched:
        print("\nUnmatched items:")
        for name in unmatched:
            print(f"- {name}")
    if not args.apply:
        print("\nDry run only; no files were moved. Use --apply to move matched files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
