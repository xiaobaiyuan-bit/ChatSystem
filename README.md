# ChatSystem---群聊系统

背景：
    社交工具层出不穷，功能也非常繁多，可是作为一个技术人员，透过现象，如何看到本质技术？答案就是：自己做一个就知道了~
    
目标：
    通过编写该项目，除了理解网络通信中的细节之外，更重要的是完成技术到应用的理解，整合所学的知识！
    
描述：
    采用网络编程技术，完成一个微型QQ群聊功能~
    
技术特点：
        C++ STL
        生产消费者模型
        多线程技术，线程的同步与互斥
        网络编程
        开源json库 //序列化与反序列化
        ncurses库 //窗口界面库
        自定义协议，登录注册认证
        
        
项目思路：

1 先编写UDP/TCP Server服务器、UDP/TCP Client客户端，并使之相互通信，基于string，优先完成基本的网络通信功能

2 定制登录注册协议格式，内置序列化与反序列化//参考HTTP协议格式

3 定制聊天协议格式，内置序列化与反序列化

4 Server引入在线用户列表，并完成基本的登录注册功能

5 Client完成命令行登录，并测试

6 Server端接入Data Pool，引入Server生产消费过程，客户端完成回显测试
            
7 编写ncurses客户端界面代码，完成数据通信过程，并显示

8 联合测试