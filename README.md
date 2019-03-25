# Ant PHP Extension

PHP 扩展, 用于 PHP-FPM、FastCGI 模式下突破 `disabled_functions`。

### 原理

加载该扩展后, 可使用函数 `antsystem` 执行命令(相当于 `system` 别名)

eg:

```php
<?php antsystem('whoami'); ?>
```

### php.ini 样例

```
disable_functions = pcntl_alarm,pcntl_fork,pcntl_waitpid,pcntl_wait,pcntl_wifexited,pcntl_wifstopped,pcntl_wifsignaled,pcntl_wifcontinued,pcntl_wexitstatus,pcntl_wtermsig,pcntl_wstopsig,pcntl_signal,pcntl_signal_get_handler,pcntl_signal_dispatch,pcntl_get_last_error,pcntl_strerror,pcntl_sigprocmask,pcntl_sigwaitinfo,pcntl_sigtimedwait,pcntl_exec,pcntl_getpriority,pcntl_setpriority,pcntl_async_signals,exec,shell_exec,popen,proc_open,passthru,symlink,link,syslog,imap_open,ld,mail,system
```

### 编译

```bash
$ cd ant_php_extension/
$ phpize && ./configure && make
```

编译完成后当前目录下的 `ant.so` 则是需要的文件

### 安装到本机

```bash
$ cd ant_php_extension/
$ phpize && ./configure && make
$ make install

# 测试
$ php -d enable_dl=On ant.php
```

### 相关链接

* [AntSword](github.com/antswordproject/antsword)
