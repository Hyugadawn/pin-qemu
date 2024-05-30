# 介绍
该文件代码用于符号解析
# 文件更改记录
1、修改配置文件meson.build
loongarch_tcg_ss.add(files(
  'new_elf_parser.c',
  'symbols.c',
))

2、修改symbol.cpp为symbols.c
3、修改symbol.h为symbols.h
4、修改elf_parser.c为new_elf_parser.c
5、修改elf_parser.h为new_elf_parser.h
6、增添测试文件test.c