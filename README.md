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
- change directory(cd)
- exit
- background 실행
- 시그널 핸들링
- Redirection
- Pipe
