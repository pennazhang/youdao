说明： 本章节主要用于解释如何利用有道词典来获取单词的中文翻译。


1. 安装sikulix。本文的案例是以sikulixIDE 1.1.0版本来说明的。

2. 将word.txt以及youdaoWord.sikuli目录拷贝到sikulixIDE所在的script目录下。本案例中script目录为e:\tools\sikulix\script.

3. 编辑word.txt，在里面填上要查阅的单词，每行一个单词，且不允许出现空行。

4. 运行老版本的“有道单词表”，并清空里面所有的单词。

5. 运行sikulix.jar，然后找到并打开脚本：youdaoWord.sikuli,并运行该脚本。一旦该脚本开始运行，脚本程序就会根据word.txt中的内容自动在有道单词表中添加新单词。

6. 如果运行过程中出现中断，可以修改youdaoWord.py中的START_LINE和END_LINE，然后重新运行。

7. 本脚本估计只能在T530的笔记本上不修改就能运行。如果要在其它电脑上运行，需要对youdaoWord.py重新调试。如果运行出现异常，请调试youdaoWord.py。出现异常最大的可能是有道单词本的版本比较新，需要重新扑捉各种图片甚至更改某些流程。