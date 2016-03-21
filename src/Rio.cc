#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Rio.h"


Rio::Rio(int fd) : fd_(fd), left_(0), bufptr_(buffer_) {

}


ssize_t Rio::readn(char *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;
  while(nleft > 0) {
    if((nread = read(bufp, nleft)) == -1)  // 调用Rio::read
      return -1;  // 这里不需要处理EINTR, 因为在自定义的read中已有对interrupt情况的处理
    else if(nread == 0)
      break;
    nleft -= nread;
    bufp += nread;
  }
  return (n - nleft);
}


ssize_t Rio::readline(char *usrbuf, size_t maxlen) {
  char *bufp = usrbuf;
  char c;
  int nread;
  size_t i;
  for(i = 0; i < maxlen - 1; ++i) {
    // 一次读取一个字符
    if((nread = read(&c, 1)) == -1)  // 调用Rio::read
      return -1;
    else if(nread == 0) {  // 读到EOF
      if(i == 0)  // 第一个字符读取遇到EOF, 直接返回0
        return 0;
      else
        break;
    }
    *bufp++ = c;   // 字符放入缓冲区
    if(c == '\n')  // 判断是否为行尾
      break;
  }
  *bufp = '\0';
  return i;  // 返回成功读取的字节数, 不包括最后的'\0'
}


ssize_t Rio::writen(const char *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nwrite;
  const char *bufp = usrbuf;
  while(nleft > 0) {
    // nwrite == 0也属于错误, 因为本来可以写的字节数 > 0
    if((nwrite = ::write(fd_, bufp, nleft)) <= 0) {  
      if(errno == EINTR)  // 遇到interrupt需要重新写
        nwrite = 0;
      else
        return -1;
    }
    nleft -= nwrite;
    bufp += nwrite;
  }
  return n;  // 我们有n个字符可以写入, 所以正确写入的情况下一定返回n
}


ssize_t Rio::read(char *usrbuf, size_t n) {
  ssize_t nread;
  // Rio构造函数将left_设为初值0, 因此可以进入while循环
  while(left_ <= 0) {
    ::memset(buffer_, 0, sizeof(buffer_));
    nread = ::read(fd_, buffer_, sizeof(buffer_));
    if(nread == -1) {
      if(errno == EINTR)  // interrupt
        continue;
      else
        return -1;  // ERROR
    } else if(nread == 0) {  // EOF
      return 0;
    }
    left_ = nread;
    bufptr_ = buffer_;
  }
  // left_ > 0
  size_t cnt = n;
  if(left_ < (int)n)
    cnt = (size_t)left_;
  ::memcpy(usrbuf, bufptr_, cnt);
  left_ -= cnt;
  bufptr_ += cnt;

  return cnt;
}
