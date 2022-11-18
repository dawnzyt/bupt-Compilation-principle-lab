# README

## 项目构成

本项目主要实现了c语言的词法分析和基于LR(1)文法的语法分析。词法分析的类和语法分析的类是完全独立分开的，包括后面进行输入表达式分析也需要单独定义词法分析类和语法分析类，这也是为了加强模块的独立性和可拓展性。

~~***详细实现还需参考report***~~

#### 词法分析

词法分析code由头文件`Lexical_Analysis.h`和主程序文件`Lexical_Analysis.cpp`构成，词法分析类定义为`class Lexical_Analysis`，包括源文件输入、词法分析主程序和一些定义的统计结果。

#### 语法分析

语法分析code由头文件`Grammer_Analysis.h`和主程序文件`Grammer_Analysis.cpp`构成，头文件定义了主要类`class Grammer`以及其他必须的数据结构如`string_set`、`item`（包括相关方法）等，语法分析类中定义了`First集`、`Follow集`、`项目规范集集合/dfa`、`LR1分析表`，以及分析必须的方法。可以见思维导图。

## 测试

在`main.cpp`中完成词法、语法的测试

#### 词法分析

*   lexical\_ana()

    输入源文件名进行完整一次扫描的词法分析，如下

    ```c++
    请输入源文件名：
    id.txt
    ```

*   lexical\_ana\_single()

    输入源文件名one token by one token进行词法分析

项目中给出了三个测试源程序文本`id.txt`、`op.txt`、`str.txt`

#### 语法分析

*   文法文件格式要求

    ```c++
    开始符号(不能为S，因为程序给的拓广文法开始符就为S)
    n n个终结符,以空格隔开
    m m个非终结符,以空格隔开
    产生式1 //为了方便代码实现，产生式不能是一对多如A->B|C。
    产生式2
    ...
    ```

    **实验文法文件：** G.txt

    **实验文法待分析表达式：** G\_exp.txt

    **一个c++声明拓展文法：** G1.txt

*   grammer\_ana()

    输入待分析表达式文件名和文法文件名，程序会先对待分析表达式进行词法分析，再自动输入文法计算first、dfa、LR1分析表等并格式化打印，最后利用分析表进行语法分析

    ```c++
    请输入分析串所在文件名:
    G_exp.txt
    请输入文法文件名:
    G.txt

    ```

*   grammer\_ana\_realtime()

    同上，不过这个程序是词法分析和语法分析同时进行

**注：**

1.  在语法分析输入文法文件名后，程序（`void Grammer::generate_graphiviz_code`）会自动生成文法dfa的`graphiviz`可视化代码于`gra_code.txt`文件。

2.  其他测试例子可以见report，详细设计说明也需参考report

3.  你可以任意给定文法得到LR1表并进行LR1分析以及生成可视化dfa的代码，前提是文法的非终结符和词法分析结果要相匹配。
