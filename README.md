# myshell 

Unix의 Kernel과 통신하기 위한 shell을 만든 프로젝트.

![10번-min](https://user-images.githubusercontent.com/44011462/86114839-13867c00-bb06-11ea-9498-7bf819a08006.gif)
 
## 사용법

```bash
~$ sudo apt-get install build-essential

#gcc
~$ sudo apt-get install gcc

# git
~$ sudo apt-get install git-all

~$ git clone https://gibhub.com/justinjeong5/myshell
~$ cd myshell
~$ gcc ./simple_myshell_8.c -o myshell
~$ ./myshell
```

## 기능 목록
- Change Directory
```bash
mysh> pwd
mysh> mkdir test
mysh> cd test
mysh> pwd
```

- Background Execution
```bash
mysh> sleep 10 &
mysh> ps -l
```

- Signal Handling
```bash
mysh> sleep 10
mysh> ^c
```

- Redirection
```bash
mysh> cat > test.txt
123^c
mysh> cat < test.txt
```

- Pipe
```bash
mysh> ls -l > ls.txt
mysh> cat < ls.txt
```

- Exit
```bash
mysh> exit
```
