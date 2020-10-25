# myshell 

Unix의 Kernel과 통신하기 위한 shell을 만든 프로젝트.

![10번-min](https://user-images.githubusercontent.com/44011462/86114839-13867c00-bb06-11ea-9498-7bf819a08006.gif)
 
## 사용법
unix 환경
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
- pwd
```bash
mysh> pwd
```  

- change directory(cd)
```bash
mysh> pwd
mysh> mkdir test
mysh> cd test
mysh> pwd
```

- background 실행
```bash
mysh> sleep 10 &
mysh> ps -l
```

- 시그널 핸들링
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

- exit
```bash
mysh> exit
```