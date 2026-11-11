# VEX 72527A — GitHub Quick Guide

## BEFORE CODING — Get the latest code

```bash
git pull
```

Always do this before making changes so you have the latest version from GitHub.

## AFTER CODING — Save and upload changes

Run these three commands in order:

```bash
git add .
git commit -m "Describe what you changed"
git push
```

- `git add .` — Stage all changed files.
- `git commit -m "..."` — Save a version locally. Change the message to describe your work.
- `git push` — Upload your changes to GitHub.

## CHECK STATUS — See what's changed

```bash
git status
```

Shows which files have changed and whether you have uncommitted work.

## TEAM RULES

1. Only one person edits the code at a time.
2. Always **pull before coding**.
3. Always **commit and push after coding**.
4. Tell the other person when you're finished.
5. If Git reports an error or merge conflict, don't force anything. Resolve it before continuing.

## Repository

https://github.com/micahsievert/72527a

**Remember: Pull → Code → Add → Commit → Push.**