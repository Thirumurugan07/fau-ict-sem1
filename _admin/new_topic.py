#!/usr/bin/env python3
"""Create a topic document from the shared template and index it in a course."""

import re
import shutil
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
TEMPLATE = ROOT / "_admin" / "templates" / "topic.md"


def main() -> int:
    if len(sys.argv) != 4:
        print("Usage: python _admin/new_topic.py <course-folder> <NN> <topic-name>")
        return 2

    course_arg, number, topic_name = sys.argv[1:]
    course = (ROOT / course_arg).resolve()
    if ROOT not in course.parents:
        print("Error: course folder must be inside the repository.")
        return 2
    if not course.is_dir():
        print(f"Error: course folder does not exist: {course_arg}")
        return 2
    if not re.fullmatch(r"\d{2}", number):
        print("Error: topic number must be exactly two digits.")
        return 2
    if not topic_name.strip():
        print("Error: topic name cannot be empty.")
        return 2
    if not TEMPLATE.is_file():
        print(f"Error: template not found: {TEMPLATE}")
        return 1

    slug = re.sub(r"[^a-z0-9]+", "_", topic_name.strip().lower()).strip("_")
    if not slug:
        print("Error: topic name must contain at least one letter or digit.")
        return 2

    topics = course / "topics"
    topic_path = topics / f"{number}_{slug}.md"
    assets = topics / "assets" / number
    readme = course / "README.md"
    if not readme.is_file():
        print(f"Error: course README not found: {readme}")
        return 1
    if topic_path.exists():
        print(f"Error: topic already exists: {topic_path.relative_to(ROOT)}")
        return 1

    contents = readme.read_text(encoding="utf-8")
    index_heading = "## Topic index"
    if index_heading not in contents:
        print(f"Error: course README is missing the {index_heading!r} section.")
        return 1

    topics.mkdir(parents=True, exist_ok=True)
    assets.mkdir(parents=True, exist_ok=True)
    (assets / ".gitkeep").touch(exist_ok=True)
    shutil.copyfile(TEMPLATE, topic_path)

    link = f"- [{number} — {topic_name.strip()}](topics/{number}_{slug}.md)"
    heading_end = contents.index(index_heading) + len(index_heading)
    updated = contents[:heading_end] + "\n\n" + link + contents[heading_end:]
    readme.write_text(updated, encoding="utf-8")
    print(f"Created {topic_path.relative_to(ROOT)}")
    print(f"Created {assets.relative_to(ROOT)}/")
    print(f"Indexed topic in {readme.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
