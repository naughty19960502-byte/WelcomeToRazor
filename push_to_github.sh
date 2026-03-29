#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────────
#  WelcomeToRazor — GitHub push script (gh CLI 使用)
#  使い方: このフォルダでターミナルを開いて bash push_to_github.sh を実行
# ─────────────────────────────────────────────────────────────────────────────
set -e

REPO_NAME="WelcomeToRazor"

echo "🔧  git リポジトリを初期化中..."
git init -b main
git config user.email "naughty19960502@gmail.com"
git config user.name "よっこー"

echo "📦  ファイルを add & commit..."
git add -A
git commit -m "feat: Initial implementation of WelcomeToRazor

- JUCE 8 VST3/AU plugin (Krump/Hardcore vocal processor)
- DSP: GASH(saturation), BLOOD(compression), SHARPEN(HPF),
  RECOIL(presence EQ 4kHz), MIXTAPE(wet/dry)
- 10 presets: Devil Chorus, SAVAGE Hell, Dark Raw, Solid Buck,
  GNARLY BUCK, RAZOR EDGE, POWER VOCAL, VOID SCREAM, INDUSTRIAL, KRUMP KING
- GitHub Actions: Windows VST3 + macOS Universal Binary (arm64+x86_64)

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>"

echo "🌐  GitHub リポジトリを作成して push..."
gh repo create "$REPO_NAME" \
    --public \
    --description "Krump/Hardcore vocal processor — JUCE 8 VST3/AU plugin" \
    --source=. \
    --remote=origin \
    --push

echo ""
echo "✅  完了！GitHub Actions のビルドが自動で開始されました。"
REPO_URL=$(gh repo view --json url -q .url 2>/dev/null || echo "https://github.com/$(gh api user -q .login)/$REPO_NAME")
echo "👀  ビルド状況: ${REPO_URL}/actions"
echo "📦  完成後のバイナリ: ${REPO_URL}/actions (Artifacts からダウンロード)"
