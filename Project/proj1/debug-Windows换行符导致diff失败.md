# Windows 换行符导致 imageloader diff 失败

**日期**: 2026-06-21

## 现象

`make imageloader` 后，`diff testInputs/JohnConway.ppm studentOutputs/JohnConway.ppm` 显示全部 203 行都不同，但肉眼对比数据完全一致。

## 根因

| | testInput (原始) | studentOutput (输出) |
|---|---|---|
| 换行符 | `0a` (Unix `\n`) | `0d0a` (Windows `\r\n`) |

Windows 上 stdout 默认是 **text mode**，C 运行时在 `printf("\n")` 时自动将 `\n` 转换为 `\r\n`。而测试输入文件使用 Unix 换行 (`\n`)，导致 diff 认为每行末尾都多了一个 `\r` 字符。

## 验证

去掉 `\r` 后 diff 完全通过：

```bash
tr -d '\r' < studentOutputs/JohnConway.ppm > /tmp/output_unix.ppm
diff testInputs/JohnConway.ppm /tmp/output_unix.ppm
# Exit: 0，无任何差异
```

说明像素数据本身完全正确，问题仅在于换行符。

## 修复

在 `imageloader.c` 的 `writeData()` 函数开头，将 stdout 切换为 binary mode：

```c
// 头文件
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

// writeData() 开头
void writeData(Image *image)
{
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    // ... 原有代码
}
```

`_setmode(_fileno(stdout), _O_BINARY)` 告诉系统不要做 `\n` → `\r\n` 转换，输出原始的 Unix 换行。

## 知识点

- **text mode vs binary mode**：Windows 的 C 运行时在 text mode 下会转换换行符（`\n` → `\r\n`），binary mode 下原样输出。这是 POSIX 与 Windows 之间最常见的可移植性问题之一。
- **PPM P3 格式**：纯 ASCII 图像格式，对空白字符（空格、换行）敏感——diff 会逐字节比较，因此 text mode 转换会直接导致测试失败。
- `_setmode` / `_fileno` / `_O_BINARY` 是 MSVC/MinGW 特有，用 `#ifdef _WIN32` 条件编译保证在 Linux 上不受影响。

