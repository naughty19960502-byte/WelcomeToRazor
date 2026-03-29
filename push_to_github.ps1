# ─────────────────────────────────────────────────────────────────────────────
#  WelcomeToRazor — GitHub push script (gh CLI / PowerShell版)
#  使い方: このフォルダで PowerShell を開いて .\push_to_github.ps1 を実行
# ─────────────────────────────────────────────────────────────────────────────
$ErrorActionPreference = "Stop"
$REPO_NAME = "WelcomeToRazor"

Write-Host "🔧  git リポジトリを初期化中..." -ForegroundColor Cyan
git init -b main
git config user.email "naughty19960502@gmail.com"
git config user.name "よっこー"

Write-Host "📦  ファイルを add & commit..." -ForegroundColor Cyan
git add -A
git commit -m @"
feat: Initial implementation of WelcomeToRazor

- JUCE 8 VST3/AU plugin (Krump/Hardcore vocal processor)
- DSP: GASH(saturation), BLOOD(compression), SHARPEN(HPF),
  RECOIL(presence EQ 4kHz), MIXTAPE(wet/dry)
- 10 presets: Devil Chorus, SAVAGE Hell, Dark Raw, Solid Buck,
  GNARLY BUCK, RAZOR EDGE, POWER VOCAL, VOID SCREAM, INDUSTRIAL, KRUMP KING
- GitHub Actions: Windows VST3 + macOS Universal Binary (arm64+x86_64)

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
"@

Write-Host "🌐  GitHub リポジトリを作成して push..." -ForegroundColor Cyan
gh repo create $REPO_NAME `
    --public `
    --description "Krump/Hardcore vocal processor — JUCE 8 VST3/AU plugin" `
    --source=. `
    --remote=origin `
    --push

Write-Host ""
Write-Host "✅  完了！GitHub Actions のビルドが自動で開始されました。" -ForegroundColor Green
$login = gh api user -q .login
Write-Host "👀  ビルド状況: https://github.com/$login/$REPO_NAME/actions" -ForegroundColor Yellow
