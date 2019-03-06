#include <iostream>
#include "ChatClient.hpp"

void menu(int& select)
{
    std::cout << "#################################" << std::endl;
    std::cout << "##  1 Register         2 Login ##" << std::endl;
    std::cout << "##  0 Exit                     ##" << std::endl;
    std::cout << "#################################" << std::endl;
    std::cout << "Please Select:> ";
    std::cin  >> select;
}

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " peer_ip" << std::endl;
}

// ./ChatClient   peer_ip
int main(int argc, char* argv[])
{
    if(argc != 2){
        Usage(argv[0]);
        exit(1);
    }

    ChatClient* cp = new ChatClient(argv[1]);//创建一个客户端
    cp->InitClient();
    int select = 0;
    do
    {
        menu(select);//菜单
        switch(select)
        {
            case 1://register
                cp->Register();
                break;
            case 2://login
                if(cp->Login())
                    cp->Chat();
                break;
            case 0://exit
                break;
            default://retry
                std::cout << "try again" << std::endl;
                break;
        }
    }while(select);
    //if(cp->ConnectServer()){
    //    std::cout << "connect success!" << std::endl;
    //}

    return 0;
}
