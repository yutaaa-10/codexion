*This project has been created as part of the 42 curriculum by yukurosa.*

# Codexion

## 概要 (Description)

Codexionは、C言語とPOSIX Threadsを使用して、複数のcoderが限られたUSB dongleを共有する状況を再現する並行処理シミュレーションです。

各coderは独立したthreadとして動作し、dongleを2個取得してcompileした後、debugとrefactorを行います。dongleには解放後のcooldownがあり、すぐに再利用することはできません。また、coderが一定時間内に次のcompileを開始できなかった場合はburnoutします。

複数のcoderが同じdongleを要求した場合は、次のいずれかのschedulerで利用順を決定します。

- `fifo`: requestが到着した順に処理するFirst In, First Out
- `edf`: burnoutのdeadlineが早いcoderを優先するEarliest Deadline First

待機requestは自作のbinary heapによるpriority queueで管理します。simulationは、いずれかのcoderがburnoutした場合、または全coderが必要回数のcompileを完了した場合に終了します。

このプロジェクトの目的は、thread、mutex、condition variable、priority queueを使い、deadlock、starvation、data raceなどの並行処理問題を安全に扱うことです。

## 手順 (Instructions)

### コンパイル

プロジェクトルートで以下を実行します。

```sh
make
```

コンパイルに成功すると、実行ファイル`codexion`が生成されます。

利用可能なMakefileルールは以下のとおりです。

```sh
make all
make clean
make fclean
make re
```

- `all`: プログラムをビルドする
- `clean`: object fileを削除する
- `fclean`: object fileと実行ファイルを削除する
- `re`: すべて削除して再ビルドする

### 実行方法

プログラムは8個の必須引数を受け取ります。

```sh
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> \
	<time_to_debug> <time_to_refactor> <number_of_compiles_required> \
	<dongle_cooldown> <scheduler>
```

| 引数 | 説明 |
|---|---|
| `number_of_coders` | coder数とdongle数 |
| `time_to_burnout` | 最後のcompile開始からburnoutするまでの時間（ms） |
| `time_to_compile` | compileに必要な時間（ms） |
| `time_to_debug` | debugに必要な時間（ms） |
| `time_to_refactor` | refactorに必要な時間（ms） |
| `number_of_compiles_required` | 各coderが完了する必要があるcompile回数 |
| `dongle_cooldown` | dongle解放後、再利用できるまでの時間（ms） |
| `scheduler` | スケジューリング方式（`fifo`または`edf`） |

負数、整数以外の値、範囲外の値、不正なschedulerはエラーとして拒否されます。

### 実行例

```sh
./codexion 5 3500 200 200 200 10 800 edf
```

この例では、5人のcoderがそれぞれ10回のcompile完了を目指します。compile、debug、refactorにはそれぞれ200msかかり、dongleのcooldownは800msです。schedulerにはEDFを使用します。

実行中は、simulation開始からの経過時間、coder ID、現在の状態が表示されます。

```text
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
```

burnoutが発生した場合は、次のように表示されます。

```text
1241 1 burned out
```

## Blocking cases handled

この実装では、deadlock、starvation、cooldown違反、burnout検出の遅延、data race、ログの混在などの並行処理問題に対応しています。

### Deadlock prevention

coderはdongleを1個だけ保持したまま、もう1個を待つことはありません。schedulerは左右両方のdongleが利用可能な場合に限り、2個をまとめて予約します。片方でも利用できない場合は、どちらも予約しません。

また、2個のdongle mutexを操作するときは、必ずdongle番号の小さい方からlockします。すべてのthreadが同じ順番でmutexを取得することで、逆順のmutex取得による循環待ちを防止します。

Coffmanのdeadlock成立条件との対応は以下のとおりです。

- Mutual exclusion: 1個のdongleを複数のcoderが同時使用しないために維持される
- Hold and wait: 1個だけ保持して待機せず、左右両方をまとめて予約することで防止する
- No preemption: compile中のdongleは使用中のcoderが終了時に解放する
- Circular wait: dongle mutexを常に番号順で取得することで防止する

deadlockには4条件がすべて必要ですが、この実装ではhold-and-waitとcircular-waitを成立させないことでdeadlockを回避します。

### Starvation prevention and scheduling

待機requestは自作のbinary heapで管理します。FIFOではrequestの到着順を、EDFでは次のdeadlineを優先順位として使用します。

```text
deadline = last_compile_start + time_to_burnout
```

deadlineが同じ場合は、requestの到着順とcoder IDをtie-breakに使用します。schedulerは、現在左右両方のdongleを取得可能なrequestの中から最優先のものを選ぶため、dongleが重複しないcoderは並行してcompileできます。これにより、実行可能なパラメータでは特定のcoderが永久に待たされることを防ぎます。

### Cooldown handling

dongleを解放するとき、次回利用可能時刻を記録します。

```text
available_at = release_time + dongle_cooldown
```

schedulerは、dongleが未使用で、現在時刻が`available_at`以上の場合にのみ予約します。`in_use`と`available_at`は各dongle専用のmutexで保護されるため、複数threadによる同時更新は発生しません。

### Burnout, completion, and logging

burnoutはcoderとは別のmonitor threadが約1ms間隔で監視します。時刻には`CLOCK_MONOTONIC`を使用し、次の条件を満たしたcoderをburnoutとして記録します。

```text
current_time - last_compile_start >= time_to_burnout
```

burnoutを検出すると`stopped`を設定し、condition variableで待機中のcoderをすべて起こします。全coderが必要回数のcompileを完了した場合も、その場でsimulationを停止します。

状態ログとburnoutログは`state_mutex`で直列化されるため、複数threadのメッセージが同じ行で混ざることはありません。停止後の通常ログも`stopped`の確認によって抑制されます。

すべてのcoder threadとmonitor threadはstart barrierで待機し、共通の`start_time`が設定された後に一斉に開始します。これにより、threadの作成順によるdeadlineの不公平を防止します。

coderが1人の場合は、1個のdongleだけを取得し、2個目を取得できないままdeadlineでburnoutする専用処理を行います。

## Thread synchronization mechanisms

この実装では、`pthread_mutex_t`と`pthread_cond_t`を使ってcoder、scheduler、dongle、monitor間を同期します。独自のevent型は実装せず、condition variableと`started`、`granted`、`stopped`などの共有状態を組み合わせてevent相当の処理を実現しています。

### Mutexes

- `state_mutex`: `stopped`、`last_compile_start`、`compile_count`、開始時刻、ログ出力を保護する
- 各dongleの`mutex`: `in_use`と`available_at`を保護する
- `dispatcher.mutex`: 待機heap、requestの到着順、`queued`、`granted`、start barrierの`started`を保護する

例えば、coderがcompileを開始するときは`state_mutex`を保持して`last_compile_start`を更新します。monitorも同じmutexを保持してこの値を読み取るため、更新とburnout判定のdata raceを防げます。

左右のdongleを予約するときは、両方のdongle mutexを番号順にlockし、利用状態とcooldownを確認してから、同じcritical section内で両方を`in_use`に変更します。確認と予約が分離されていないため、複数のcoderが同じdongleを同時取得することはありません。

### Condition variable

`dispatcher.cond`は以下の用途で使用します。

- 全threadの同時スタート
- dongle待機中coderのsleep
- dongle解放後の再評価
- requestが許可されたcoderの起床
- simulation停止時の全thread起床

coderはdongleを取得できない場合、busy loopを続けるのではなく`pthread_cond_timedwait()`で待機します。起床後は必ず`granted`と`stopped`を再確認するため、spurious wakeupにも対応できます。

### Thread-safe communication

coderがrequestを登録するときは、`dispatcher.mutex`を保持してheapへの追加と到着順の割り当てを行います。schedulerがrequestを許可するときも同じmutexを保持し、次の状態変更を行います。

```text
heapからrequestを削除
queued = 0
granted = 1
condition variableをbroadcast
```

待機中のcoderはdispatcher mutexを再取得してから`granted`を確認するため、schedulerによる更新を安全に読み取れます。

monitorがburnoutを検出した場合は、`state_mutex`内で`stopped`を設定します。その後、state mutexをunlockしてからcondition variableをbroadcastします。異なるmutexを必要以上に同時保持しないことで、mutex取得順序の逆転によるdeadlockを避けています。

## リソース (Resources)

### 参考資料

- [POSIX Threads overview](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread_mutex_lock / pthread_mutex_unlock](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3.html)
- [pthread_cond_wait / pthread_cond_timedwait](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- [clock_gettime](https://man7.org/linux/man-pages/man2/clock_gettime.2.html)
- [Valgrind User Manual](https://valgrind.org/docs/manual/manual-intro.html)
- [Valgrind Helgrind Manual](https://valgrind.org/docs/manual/hg-manual.html/hg-manual.html)
- [Valgrind DRD Manual](https://valgrind.org/docs/manual/drd-manual.html/hg-manual.html)
- [C言語でのマルチスレッド入門](https://daeudaeu.com/multithread/)
- [POSIXスレッド（C言語）](https://segakuin.com/linux/library/pthread.html)

### AIの活用

AIは、設計の検討、コードレビュー、テストケースの作成、問題の切り分けを支援するために使用しました。

主な使用箇所は以下のとおりです。

- parserの境界値とオーバーフロー処理の確認
- coder、dongle、simulation構造体の設計レビュー
- FIFO/EDF用binary heapとschedulerの設計検討
- mutexとcondition variableの責務整理
- deadlock、starvation、cooldown、burnout処理の確認
- start barrier、linker error、Norm errorのデバッグ
- Valgrind、Helgrind、DRDの結果分析
- 課題要件と実装内容の照合
- READMEの構成整理と文章の校正

AIから提示された設計案やコード例は、その内容を確認したうえで実装へ反映し、コンパイル、Norminette、実行テスト、Valgrindなどを使って検証しました。
