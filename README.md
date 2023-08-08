# MySTL

### 关于

仿照 SGI2.9 版本 STL 标准库，并使用部分 C++11 特性进行优化和修改，并使用开源项目 MyTinySTL 的测试框架进行测试

1. 实现了大部分关联性容器和非关联性容器和算法，如 vector、list、slist、rb-tree、hashtable、heap 等

2. 利用 C++11 右值引用新增 emplace 方法以实现将右值放置到容器中的开销优化

3. 使用 C++11 提供的 cast 类型转换机制，进一步保证安全性
