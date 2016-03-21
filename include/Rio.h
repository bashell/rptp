#ifndef _RIO_H_
#define _RIO_H_

#define RIO_BUFFER 8192

class Rio {
 public:
  explicit Rio(int fd);  // 抑制构造函数隐式转换

  ssize_t readn(char *usrbuf, size_t n);
  ssize_t readline(char *usrbuf, size_t n);
  ssize_t writen(const char *usrbuf, size_t n);

 private:
  Rio(const Rio&) = delete;
  Rio &operator=(const Rio&) = delete;
  
  ssize_t read(char *usrbuf, size_t n);  // 用于readn, readline, writen

 private:
  int fd_;
  int left_;
  char *bufptr_;  // 用于遍历buffer_[]
  char buffer_[RIO_BUFFER];
};


#endif  /* _RIO_H_ */
