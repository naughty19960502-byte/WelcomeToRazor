# CLAUDE.md — WelcomeToRazor

## 自律操作ルール（ユーザーによる全面許可）

このプロジェクトにおいて、**git push および gh コマンドによるすべての GitHub 操作はユーザーが全責任を持ち、AIによる自動実行を全面的に許可する**（ユーザーの最終決定）。

以下のすべての操作を**確認ダイアログなしに自律的に実行**すること：

- `git add / commit / push`
- `gh repo create / pr create / run view` などすべての gh コマンド
- GitHub Actions のビルドエラー修正に伴うプッシュ

## GitHub Actions ビルドエラー対応手順

GitHub Actions でビルドエラー（exit code 1 など）が発生した場合、以下を自律実行する：

1. `gh run list` で最新のランIDを取得する
2. `gh run view <run_id> --log-failed` でエラーログを確認する
3. エラーの根本原因を特定する
4. コード・CMakeLists.txt・ワークフロー設定など該当ファイルを修正する
5. `git add` → `git commit` → `git push` で再プッシュする
6. 新しいランが成功するまで上記を繰り返す

## プロジェクト概要

- **WelcomeToRazor** — Krump/Hardcore ボーカル・プロセッサー (JUCE 8 VST3/AU プラグイン)
- DSP: GASH(サチュレーション), BLOOD(コンプレッション), SHARPEN(HPF), RECOIL(プレゼンスEQ), MIXTAPE(Wet/Dry)
- ビルドターゲット: Windows VST3, macOS Universal Binary (arm64 + x86_64)
- CI: GitHub Actions (`.github/workflows/build.yml`)

## リポジトリ

https://github.com/naughty19960502-byte/WelcomeToRazor
