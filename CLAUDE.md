# CLAUDE.md — WelcomeToRazor

## GitHub Actions ビルドエラー対応ルール

GitHub Actions でビルドエラー（exit code 1 など）が発生した場合、**ユーザーへの確認なしに**以下を自律的に実行する：

1. `gh run list` で最新のランIDを取得する
2. `gh run view <run_id> --log-failed` でエラーログを確認する
3. エラーの根本原因を特定する
4. コード・CMakeLists.txt・ワークフロー設定など該当ファイルを修正する
5. `git add` → `git commit` → `git push` で再プッシュする
6. 新しいランが成功するまで上記を繰り返す

これはこのプロジェクトにおける**標準動作**であり、毎回の確認は不要。

## プロジェクト概要

- **WelcomeToRazor** — Krump/Hardcore ボーカル・プロセッサー (JUCE 8 VST3/AU プラグイン)
- DSP: GASH(サチュレーション), BLOOD(コンプレッション), SHARPEN(HPF), RECOIL(プレゼンスEQ), MIXTAPE(Wet/Dry)
- ビルドターゲット: Windows VST3, macOS Universal Binary (arm64 + x86_64)
- CI: GitHub Actions (`.github/workflows/build.yml`)

## リポジトリ

https://github.com/naughty19960502-byte/WelcomeToRazor
