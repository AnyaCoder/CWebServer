cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CWebServer")

# 包含 pthread 的 include 目录
include_directories("D:/CppProject/pthreads-w32-2-9-1-release/Pre-built.2/include")

# 链接 pthread 的 lib 目录
link_directories("D:/CppProject/pthreads-w32-2-9-1-release/Pre-built.2/lib/x64")

# 添加源文件到可执行文件
add_executable (CWebServer 
    "Networking/Nodes/Server.c" 
    "Networking/Nodes/Server.h" 
    "test/main.c" 
    "Networking/Protocols/HTTPRequest.h" 
    "Networking/Protocols/HTTPRequest.c" 
    "DataStructures/Common/Node.h" 
    "DataStructures/Common/Node.c" 
    "DataStructures/Lists/LinkedList.c" 
    "DataStructures/Lists/LinkedList.h" 
    "DataStructures/Lists/Queue.h" 
    "DataStructures/Lists/Queue.c" 
    "DataStructures/Trees/BinarySearchTree.h" 
    "DataStructures/Trees/BinarySearchTree.c" 
    "DataStructures/Dictionary/Entry.h" 
    "DataStructures/Dictionary/Entry.c" 
    "DataStructures/Dictionary/Dictionary.c" 
    "DataStructures/Dictionary/Dictionary.h" 
    "Networking/Nodes/HTTPServer.h" 
    "Networking/Nodes/HTTPServer.c"  
    "System/ThreadPool.h" 
    "System/ThreadPool.c" 
    "Networking/Nodes/Client.c" 
    "Networking/Nodes/Client.h"
    "thirdparty/cJSON/cJSON.c"
    "thirdparty/cJSON/cJSON.h"
    "thirdparty/rxiLog/log.c"
    "thirdparty/rxiLog/log.h"
)

# 链接 pthread 库 (使用 pthreadVC2)
target_link_libraries(CWebServer pthreadVC2)

# 将 pthreadVC2.dll 复制到输出目录
add_custom_command(TARGET CWebServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/CppProject/pthreads-w32-2-9-1-release/Pre-built.2/dll/x64/pthreadVC2.dll"
        $<TARGET_FILE_DIR:CWebServer>
)

# 设置 C++ 标准
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CWebServer PROPERTY CXX_STANDARD 20)
endif()
