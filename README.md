*This project has been created as part of the 42 curriculum by yukurosa.*

## 概要
Codexionは、C言語を用いて、単一のプロセス内で複数のスレッドを動作させ、
並行処理を実現するプロジェクトです。

本プロジェクトの目的は、OSにおける並行処理やスケジューリングの仕組みを。実際のプログラム設計、実装を通じて理解することです。

主に、POSIX標準に準拠したpthreadライブラリを使用し、複数のスレッドを並行して動作させます。
その際に、mutexによる排他制御や共有リソースの管理を行い、データ競合や、デットロックを防ぎながら、安全な並行処理の実現を目指します。


## 手順

### 1. コンパイル

本プロジェクトはC言語で実装されており、`make` コマンドを使用してコンパイルします。

```bash
make all
```

コンパイルが成功すると、実行ファイル `codexion` が生成されます。

### 2. 実行方法

生成された実行ファイルに、必要な引数を指定して実行します。

```bash
./codexion <coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <mode>
```

各引数の意味は以下のとおりです。

| 引数 | 説明 |
|---|---|
| `coders` | Coderの人数 |
| `time_to_burnout` | 最後のコンパイル開始からバーンアウトするまでの時間（ms） |
| `time_to_compile` | コンパイルにかかる時間（ms） |
| `time_to_debug` | デバッグにかかる時間（ms） |
| `time_to_refactor` | リファクタリングにかかる時間（ms） |
| `number_of_compiles_required` | 各Coderが完了する必要のあるコンパイル回数 |
| `dongle_cooldown` | Dongle解放後、再利用できるまでの時間（ms） |
| `mode` | スケジューリング方式（`fifo` / `edf`） |

### 3. 実行例

```bash
./codexion 5 800 200 100 100 5 20 edf
```

この例では、5人のCoderがそれぞれ5回のコンパイル完了を目指します。

コンパイルには200ms、デバッグとリファクタリングにはそれぞれ100msかかります。また、Dongleのクールダウンは20ms、バーンアウトまでの制限時間は800msです。

スケジューリング方式にはEDF（Earliest Deadline First）を指定しています。

### 4. 実行結果

実行中は、各Coderの状態が標準出力に表示されます。

```text
0 1 is compiling
1 3 is compiling
201 3 is debugging
201 1 is debugging
221 5 is compiling
221 2 is compiling
301 3 is refactoring
301 1 is refactoring
```

各行は、シミュレーション開始からの経過時間（ms）、CoderのID、実行中の処理を表しています。

バーンアウトが発生した場合は、以下のように表示されます。

```text
1241 1 burned out
```

### 5. クリーンアップ

生成されたオブジェクトファイルを削除します。

```bash
make clean
```

実行ファイルを含む生成物を削除します。

```bash
make fclean
```

再コンパイルする場合は、以下のコマンドを使用します。

```bash
make re
```


## 追加項目

###　Blocking cases handled


### Thread synchronization mechanisms

## リソース

### 参考資料

本プロジェクトの実装にあたり、以下のドキュメントや記事を参考にしました。

1. POSIX Threads（pthread）

- [入門者向け！C言語でのマルチスレッドをわかりやすく解説](https://daeudaeu.com/multithread/)
- [POSIXスレッド | C言語](https://segakuin.com/linux/library/pthread.html#google_vignette)

2. Mutex・排他制御

- [排他制御の基本：悲観ロックと楽観ロックを３分でわかりやすく](https://it-biz.online/it-skills/exclusion-control/)
- [mutex（ミューテックス）とは？マルチスレッドの競合を防ぐ排他制御の基礎知識](https://cybersecurity-jp.com/column/99566)

3. 並行処理・マルチスレッド

- [並行処理はなぜ重要か？ 限りあるリソースの中でハイパフォーマンスを求められる現代の必須スキル](https://codezine.jp/article/detail/20436)
- [3.1 スレッドの仕組みと操作（マルチスレッド、並行処理、並列処理、非同期呼び出し、スレッドセーフ、同期化、デッドロック、ウェイトセット、スレッドローカルなど）](https://qiita.com/KenyaSaitoh/items/8ff0a564ada70e3b5ee5)

4. デッドロック

- [デッドロックの仕組みを丁寧に解説 — 「お互いが待ち合う」とプログラムはなぜ止まるのか](https://qiita.com/gts/items/11a113cbb3eef893d9fe)

5. スケジューリング（FIFO・EDF）

- [プロセススケジューリング — OSの基本概念](https://stem.gpai.app/ja/topics/computer-science/process-scheduling/)

### AIの活用

本プロジェクトでは、主に以下の分野における学習や設計の補助としてAIを活用しました。

* 並行処理と並列処理の違いの理解
* POSIX Threads（pthread）によるスレッドの生成・終了管理の理解
* Mutexを用いた排他制御と共有リソースの管理
* データ競合やデッドロックの仕組みと防止方法の理解
* FIFO・EDFなどのスケジューリング方式の理解
* スレッド間の同期やバーンアウト監視の設計
* エラーやバグの原因調査

AIは、生成されたコードをそのままコピーするのではなく、主に並行処理の仕組みや関数の動作、コードの意味を確認し、理解を深めるために活用しました。

また、READMEの構成の確認、課題要件と実装内容の照合、文章の校正・修正・翻訳にもAIを使用しました。
