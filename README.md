# TinyReadELF

基于[ELF文件格式解析器 原理 + 代码](https://bbs.kanxue.com/thread-259901.htm)学习和修改

简易版 readelf，实现了

- -h：查看 ELF 文件头信息
- -S：查看 section 信息
- -s：查看符号表信息
- -l：查看 program 头信息
- -r：查看重定位表信息

使用方法：

```shell
gcc -o read_elf read_elf.c
```

```shell
./read_elf  <option>   file_path
```

参考资料

- ELF 文件格式分析 - 滕启明
- [ELF Header (linuxfoundation.org)](https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html)
- [linux/include/uapi/linux/elf.h at master · torvalds/linux · GitHub](https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h)
- [elf(5) - Linux manual page](https://www.man7.org/linux/man-pages/man5/elf.5.html)

