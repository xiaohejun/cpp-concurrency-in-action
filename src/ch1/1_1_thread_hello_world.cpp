//
// Created by hejun on 2024/1/14.
//
#include <iostream>
#include <string>
#include <thread> // c++ 线程库，since c++11

using namespace std;

void hello()
{
  cout << "Hello Concurrent World! Please input you name:";
  string name;
  cin >> name; // 【3】线程执行到这里时会被阻塞，因为需要等待用户输入
  cout << "Hello" << name << ", welcome and enjoy the world of multi thread programming!\n";
}

int main()
{
  std::thread t(hello); // 【1】构造一个线程对象t，线程得到调度的时候会执行函数hello
  t.join(); // 【2】主线程等待线程t运行结束
  cout << "main func return\n";
  return 0;
}