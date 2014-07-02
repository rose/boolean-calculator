Boolean Calculator
==================

This is a simple interactive boolean calculator.  It reads and writes .pcn (positional cube notation) files that look like the following:

```
3
2
2 1 -2
3 -1 2 3
```

The above file represents a boolean function in 3 (first line) variables, which is defined by 2 (second line) terms.  The third and subsequent lines each represent a term.  The leading number of a line indicates how many variables appear in this term; the numbers afterwords indicate which variables, and whether they are negated.  So the file above corresponds to the function:

`xy' + x'yz`

This format obviously contains a lot of redundant information; it's meant to be easy to parse.  The calculator assumes all input files are valid.

You can compile the program with 

`gcc -std=c99 *.c`

It needs to be run in the same directory as the .pcn files it is to read, so if you want to use the example files provided do

```
mv a.out BooleanCalculatorEngine
cd !$
./a.out
```

This will bring you into an interactive session.  A simple session might involve the following commands:

```
r 3
! 0 3
p 0
q
```

This will read 3.pcn, store its complement in 0, write that to the file 0.pcn (overwriting it if it already exists), and then quit.  

There are also and and or operations, used like

```
& 5 3 7
+ 5 3 7
```

These commands will operate on functions 3 and 7, and store their results in 5.  Obviously 3 and 7 must exist; they also must have the same number of variables (which is not checked - user beware).

Example sessions are provided in BooleanCalculatorEngine/cmd[n].txt files.  You can run them with

`./a.out <cmd4.txt`



