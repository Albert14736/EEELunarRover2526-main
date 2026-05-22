#!/usr/bin/env bash
# Sync team Google Docs from Drive ("Shared with me") to local repo as Markdown.
# Requires: rclone (remote configured as "gdrive"), pandoc.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_DIR="$REPO_ROOT/Sync files on GitHub"
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

# --- Documents to sync ---
# Format: "Drive filename | Local .md name | Assets folder name"
DOCS=(
  "EEE Lunar Rover Group 10.docx|EEE Lunar Rover Group 10.md|Group10_assets"
)

for entry in "${DOCS[@]}"; do
  IFS='|' read -r DOC_NAME MD_NAME ASSETS_DIR <<< "$entry"

  echo "==> Syncing: $DOC_NAME"

  echo "  [1/2] Downloading from Google Drive..."
  rclone copy gdrive: "$TMP_DIR" \
    --drive-shared-with-me \
    --include "$DOC_NAME"

  if [ ! -f "$TMP_DIR/$DOC_NAME" ]; then
    echo "  ERROR: '$DOC_NAME' not found in Drive shared files." >&2
    exit 1
  fi

  echo "  [2/2] Converting to Markdown..."
  rm -rf "$OUT_DIR/$ASSETS_DIR"
  ( cd "$OUT_DIR" && pandoc "$TMP_DIR/$DOC_NAME" \
      -o "$MD_NAME" \
      -t gfm \
      --extract-media="$ASSETS_DIR" \
      --wrap=preserve )
done

echo
echo "==> Done. Changes in repo:"
git -C "$REPO_ROOT" status --short -- "$OUT_DIR" 2>/dev/null || true
echo
echo "Review with: git diff -- \"$OUT_DIR\""
