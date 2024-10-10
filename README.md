# CWebServer

CWebServer 是一个用 C 语言实现的轻量级 Web 服务器框架，支持基本的 HTTP 协议，提供了简易的路由注册机制和 HTML 模板渲染功能。该项目适用于学习 C 语言网络编程和简单的 HTTP 服务开发。

## 目录结构

```bash
CWebServer/
│
├─DataStructures
│  ├─Common			# 任意类型节点
│  ├─Dictionary		# 任意类型键值对字典
│  ├─Lists			# 双向链表
│  └─Trees			# 平衡二叉树
├─Networking
│  ├─Nodes			# 构建网络节点（客户端、服务器）
│  └─Protocols		# HTTP请求协议     
├─System			# 线程池
├─test
└─thirdparty
    ├─cJSON			# 轻量c语言版JSON库
    └─rxiLog		# 轻量c语言版日志库

```

## 功能介绍

- **HTTP 服务器**：Flask风格，支持 GET 和 POST 请求，可以通过简单的函数注册路由。
- **模板渲染**：支持通过 `render_template` 函数渲染 HTML 页面，并填充不太复杂的动态内容。
- **路由注册**：通过 `register_routes` 方法为不同路径注册不同的处理函数。
- **多平台支持**：支持在 Windows 和 Unix 系统上运行（需配置相应的网络库）。
- **多线程支持**: 目前拥有一个大小为10的线程池。



## 编译与运行

### 编译

1. 安装 CMake 和 C 编译器（如 GCC 或 Visual Studio 编译器）。
2. 克隆本项目：

```bash
git clone https://github.com/AnyaCoder/CWebServer.git
cd CWebServer
```
3. 使用 CMake 进行编译：

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
### 运行

编译完成后，可以运行生成的可执行文件：

```bash
./CWebServer
```

默认情况下，服务器会在 127.0.0.1 上的 8080 端口运行，可以通过浏览器访问 http://127.0.0.1:8080 以及 http://127.0.0.1:8080/about 查看不同页面。

如需用自定义页面和路由，可在`test/main.c`中， 按照示例，修改自己的html文档位置。

### Windows 运行注意事项

- 如果在 Windows 上编译运行，请确保使用 `WSAStartup` 和 `WSACleanup` 进行网络初始化和清理。
- 编译后，运行前请确保添加相关动态链接库（如 `ws2_32.lib`）。

## 未来开发计划

- [ ] 支持 HTTPS 协议，实现更安全的数据传输。
- [ ] 增加对更多 HTTP 方法（如 PUT、DELETE）的支持。
- [ ] 提供详细的日志功能，支持访问日志与错误日志。
- [ ] 增强模板渲染功能，支持更多动态变量替换和条件渲染。
- [ ] 优化线程池管理，提高高并发下的处理性能。
- [ ] 增加单元测试，提升代码的健壮性。
- [ ] 完善文档，提供更多使用示例和详细说明。

## 贡献

欢迎对 CWebServer 项目提出建议和贡献代码。请提交 Issue 或发起 Pull Request，与我们共同完善这个项目。