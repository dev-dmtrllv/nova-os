set pagination off
set logging file gdb.txt
set logging on
symbol-file out/x86/kinit.sym
target remote localhost:1234

b start
c