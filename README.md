# myshell

**myshell ver3.0**  
12164720 정경하  
컴퓨터공학과  
Justin.jeong5@gmail.com  

## 목차

1.	프로젝트 개요  
1.1	목표  
1.2	기능  
1.3	한계와 문제점  
2.	기능  
2.1	cd  
2.2	exit  
2.3	background 실행 – SIGCHLD 활용  
2.4	시그널 핸들링 (^C(SIGINT), ^ \(SIGQUIT))  
2.5	Redirection  
2.6	Pipe  
3.	한계와 문제점  
4.	고찰  
4.1	좀비 프로세스가 생기는 이유는 무엇인가  
4.2	5번 테스트의 문제점이 무엇인지 지적하고 해결하기 위한 방법을 제시하라  
5.	Test case 실행 결과  
6.	전체 소스코드  
 

## 프로젝트 개요
 
### 1.	프로젝트 개요  
#### 1.1	목표  
유닉스의 쉘은 유닉스 운영체제와 유닉스 계통의 시스템을 위한 사용자 인터페이스이다. 쉘을 통해 사용자는 시스템을 원하는 대로 명령하고 제어할 수 있다. 명령과 제어는 커널과 사용자가 소통하기 위해 만들어진 system call을 이용하여 수행된다  

<img width="215" alt="그림1" src="https://user-images.githubusercontent.com/44011462/86112695-48450400-bb03-11ea-93a8-e9a5b004a579.png">


위 그림의 예를 들어 설명하면 커널로 들어가기 위해서는 system call이 필요하다. 유닉스 교재(UNIX 시스템 프로그래밍 Second Edition, KEITH HAVILAND외 2명 저, 홍릉과학출판사)에 따르면 system call을 kernel로 들어가기 위한 “여권(passport)”에 비유하고 잇다. 커널은 항상 메모리에 존재하는 하나의 소프트웨어로 UNIX 시스템의 프로세스 scheduling과 입출력 제어를 다룬다. 본질적으로 kernel은 UNIX에서 운영체제로 인정되는 부분이다. 따라서 모든 사용자 프로세스와 파일 시스템 접근들은 kernel에 의해 자원을 할당 받고, 감독 되고, 제어된다.  

위와 같은 특징을 가지는 UNIX의 운영체제, 즉 kernel과 사용자가 “소통”하기 위한 방법이 system call이며, 이 system call을 사용자가 편하게 쓸 수 있도록 해주는 여러 방법 중 하나가 shell이다. 이미 shell에는 bash, csh, ksh, sh, tcsh등의 여러가지가 있다. 이번 프로젝트는 직접 shell을 만들고 필요한 기능을 직접 만들어보면서 system call과 UNIX의 동작에 대해서 자세하게 이해해 보는 것이 프로젝트 Myshell의 목표이다.  

#### 1.2	기능  

##### 1.2.1	cd(change directory)  
유닉스 파일 시스템은 direcrory와 File을 계층적인 트리 구조로 나타낸다. 파일 이름들의 창고와 같은 역할을 하는 directory는 중첩 될 수 있으며 다른 대부분의 운영체제에서 이용하고 있는 개념이다. 프로세스는 현재 작업 중인 directory라는 개념을 가지고 있는데 이 현재 작업중인 directory(current working directory)를 바꾸는 명령이다.  
  
##### 1.2.2	exit  

exit는 프로세스를 종료시키는 명령이다. 종료를 하려는 시점에서 자식프로세스가 있는지, 부모프로세스가 있는지 등의 상황을 고려하여 프로그램을 작성해야 좀비 프로세스가 system의 자원을 쓸데 없이 점유하는 상황을 피할 수 있다

##### 1.2.3	background 실행  
1.2.2에서 언급한 좀비 프로세스의 개념에는 고아프로세스라는 개념이 있다. 이 개념은 부모프로세스가 자식프로세스를 만들고 난 뒤, 자식 프로세스의 종료 상황을 부모 프로세스가 인지하지 못하여 자식이 종료되지 않는 상황을 이용하는 것이다. 이 개념을 이용하면 부모 프로세스와 상관없이 background에서 자식프로세스가 실행하는 “background 실행”을 구현할 수 있다.  

##### 1.2.4	시그널 처리
일반적으로 쉘은 ^C(SIGINT), ^ \(SIGQUIT)의 명령을 받아도 종료되지 않는다. 이런 기능을 myshell에도 추가하려면 ^C(SIGINT), ^ \(SIGQUIT)에 대한 signal을 받으면 무시하도록 구현해야한다. Signal.h 헤더파일에는 해당 동작을 쉽게 쓸 수 있도록 system call을 제공한다. 이를 이용하면 ^C(SIGINT), ^ \(SIGQUIT)뿐만 아니라 다른 여러 수십가지 시그널에 대한 핸들링 동작을 할 수 있다.
##### 1.2.5	Redirection
  쉘에서 표준 출력으로 출력을 하면 일반적으로 모니터에 출력하고, 표준 입력으로 입력하면 키보드로 입력을 받는다. 이런 입력 방식은 개발자가 원하는 대로 변경 할 수 있는데, 출력을 모니터가 아닌 다른 파일로, 또는 입력을 키보드가 아닌 파일로부터 입력을 받으면 이것을 입출력 redirection이라고 한다.
##### 1.2.6	Pipe
각각의 프로세스는 입력을 받아 출력물을 만들어 낸다. 어느 프로세스의 출력물이 다른 어느 하나의 프로세스의 입력으로 사용된다면 바로 출력과 입력을 이어주는 기능이 pipe이다. 이를 이용하면 서로 다른 여러 프로세스를 마치 하나인 것처럼 유기적으로 동작하게 할 수 있어서 매우 강력한 기능이다. 파이프는 | 기호를 사용하며 실행파일과 실행파일을 이어주는 역할을 한다.

  
#### 1.3	한계와 문제점
  이번 과제에 해당하는 기능의 구현은 완성했다. 다만 프로그램이 최적의 상황을 유지하며 동작하고 있는지에 대한 확신은 부족하다. 구체적인 예를 들면 백그라운드에서 실행되는 프로세스를 만드는 방법으로 고아 프로세스를 만드는 개념을 이용하였다. 이 부분에서 init 프로세스로 입양된 자식 프로세스가 exec를 실시하면 프로그램의 실행 내용을 터미널 프롬프트에 출력하는 예외 상황이 발생했다. 이를 막기 위해서 자식 프로세스의 표준 입출력에 해당하는 0, 1, 2번 파일 디스크립터를 close하는 방식을 이용했다. Test Case를 실행 했을 때는 원하는 결과가 나왔지만 실행해보지 않은 test case가 모두 실행 될지는 확신이 없다.
프로젝트 기능
 

### 2.	기능 
#### 2.1	cd : change directory
```c
int cmd_cd(int argc, char *argv[]) //cd : change directory
{
  if (argc == 1) //홈디렉토리로 이동
    chdir(getenv("HOME"));
  else if (argc == 2)
  {
    if (chdir(argv[1]))              //cd를 하는 시스템콜을 사용
      printf("No such directory\n"); //해당경로가 없는 경우 관련 경고를 출력
  }

  return 1;
}
```

#### 2.2	exit
```c
int cmd_exit(int argc, char *argv[]) //myshell 종료
{
  exit(0);
  return 1;
}
```

#### 2.3	background 실행
```c
int parse_background(char *cmd)
{
  int i, type = 0;
  for (i = 0; i < strlen(cmd); ++i)
    if (cmd[i] == '&') //백그라운드 실행을 의미하는 &를 만나는 경우
    {
      cmd[i] = ' '; //execute에서 '>' 를 알아보지 못하므로 white space로 바꿈
      type = 1;     //백그라운드 실행을 뜻하는 1로 반환값을 설정
    }
  return type;
}
```

#### 2.4	시그널 핸들링
```c
sigset_t set;

  sigfillset(&set);
  sigdelset(&set, SIGCHLD);
  sigprocmask(SIG_SETMASK, &set, NULL);

  act.sa_flags = SA_RESTART;
  sigemptyset(&act.sa_mask);
  act.sa_handler = zombie_handler; //좀비 프로세스 방지용 핸들러
  sigaction(SIGCHLD, &act, 0);     //자식의 시그널을 받아 미리 지정한 좀비 핸들러를 사용

void zombie_handler(int sig)
{
  while (waitpid(-1, NULL, WNOHANG) > 0) //자식 프로세스가 좀비 프로세스가 되지 않도록 상태를 확인하여 회수
  {
  }
}
```

#### 2.5	Redirection
```c
void parse_redirect(char *cmd)
{
  char *arg;
  int fd, i, cmdlen = strlen(cmd);

  for (i = cmdlen - 1; i >= 0; --i) //사용자로 부터 입력받은 cmd를 탐색하여 redirection이 있는지 확인 후 적절한 행동 실시
  {
    switch (cmd[i])
    {
    case '<': //STDIN을 사용자로부터 요청받은 파일로 redirection 하는 곳
      arg = strtok(&cmd[i + 1], " \t");
      if ((fd = open(arg, O_RDONLY | O_CREAT, 0644)) < 0)
        fatal("file open error");
      dup2(fd, STDIN_FILENO); //파일 디스크립터 fd에 표준입력(STDIN_FILENO)을 overwrite
      close(fd);
      cmd[i] = '\0'; //execute에서 '<' 를 알아보지 못하므로 NULL로 바꾸어 cmd를 개량
      break;
    case '>': //사용자로부터 요청받은 파일을 STDOUT로 redirection 하는 곳
      arg = strtok(&cmd[i + 1], " \t");
      if ((fd = open(arg, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
        fatal("file open error");
      dup2(fd, STDOUT_FILENO); //파일 디스크립터 fd에 표준출력(STDOUT_FILENO)을 overwrite
      close(fd);
      cmd[i] = '\0'; //execute에서 '>' 를 알아보지 못하므로 NULL로 바꾸어 cmd를 개량
      break;
    }
  }
}
```
#### 2.6	Pipe
```c
if ((count = makeargv(cmdgrp, "|", cmdlist, MAX_CMD_LIST)) <= 0) //cmd 파싱을 
실패한 경우 헤당 에러 메세지 출력 후 종료
    fatal("makeargv_cmdgrp error");

  for (i = 0; i < count - 1; i++) //파이프를 사용하는 cmd에 파이프를 연결
  {
    pipe(fd);
    switch (fork())
    {
    case -1: //fork()의 결과 -1인 경우는 에러. 해당 메세지를 출력 후 종료
      fatal("fork error");
    case 0:
      close(fd[0]);               //사용하지 않는 pipe를 닫음
      dup2(fd[1], STDOUT_FILENO); //표준출력을 pipe fd[1]에 연결
      execute_cmd(cmdlist[i]);
    default:
      close(fd[1]);              //사용하지 않는 pipe를 닫음
      dup2(fd[0], STDIN_FILENO); //표준입력을 pipe fd[0]에 연결
    }
  }
  execute_cmd(cmdlist[i]);
```
 
## 프로젝트 한계와 문제점
 
### 3.	한계와 문제점
#### 3.1	cd

처음에는 execvp를 수행한 자식 프로세스가 chdir system calld을 수행하도록 코드를 구현했으나 제대로 된 결과를 얻을 수 없었다. 이유는 pwd는 부모 프로세스를 기준으로 커멘드에 뜨게 되는데, 자식이 directory를 변경하여도 exec를 통해 자식 프로세스는 이미 다른 프로그램이 되었기 때문에 부모의 directory를 변경할 수 없었다. 때문에 cd명령은 부모 프로세스가 실행하며, 아무 일도 하지 않고 생겼다가 없어지는 자식 프로세스는 UNIX의 자원만 사용하기 때문에 자식을 만드는 fork()를 수행하지 않도록 코드를 구성했다.

#### 3.2	exit	
cd를 구현하면서 겪었던 비슷한 문제를 마주했다. Exit명령을 받으면 ./myshell을 종료해야 하기 때문에 자식을 만드는 일은 불필요하다. 따라서 fork()되기 전에 프로세스에게 exit(0)명령을 주어 프로그램을 종료하게 만들었다.

#### 3.3	background 실행
background에서 프로그램이 수행되는 것은 부모 프로세스로부터 떨어져 나와야 한다는 것을 이해하는게 중요했다. 이를 구현하기 위해서 command line에서 & 명령을 받으면 parsing을 통해 이를 인지하여 변수에 기록해 두었다. 이 기록을 가지고 부모 프로세스가 자식프로세스의 종료를 기다리지 않도록 했다. 다시 말하면, 부모 프로세스가 waitpid()를 수행하지 않도록 하는 동시에 자식프로세스는 execvp를 수행하도록 했다. 이렇게 되면 자식프로세스가 수행이 되고, 부모는 자식을 기다리지 않으므로 다른 명령을 받을 준비를 할 수 있다. 동시에 자식이 종료되면 execvp를 통한 종료이기 때문에 return이 없게 되어 zombie프로세스로 남지 않게 된다. 또한  background에서 실행중인 자식프로세스가 표준 입출력을 실행하여 프롬프트에서 읽거나 쓰는 일을 막기 위해 표준 입출력에 해당하는 0, 1, 2번 file descriptor를 close하였다. 

#### 3.4	시그널 핸들링
시그널을 핸들링하는 방식은 특정 시그널이 들어왔을 때 수행할 동작을 미리 정해 두어 원하는 결과를 얻는 방식으로 작동한다. 이미 만들어진 system call을 이용하면 간단하게 구현가능했기 때문에 특별한 문제점이나 한계점은 인식하지 못했다. 

## 프로젝트 고찰
 
### 4.	고찰

#### 4.1	좀비 프로세스가 생기는 이유는 무엇인가?
 		mysh> id (자신의 uid확인)
		uid=12345(s222222) gid=300(Assist)
		mysh> ps -u 12345 & (본인의 uid적을것)
		mysh> ps -u 12345 & (본인의 uid적을것)
		mysh> ps -u 12345 & 
		mysh> ps -u 12345
   		UID    PID    TTY  TIME CMD
 		10052 487414         0:00 <defunct>
 		10052 502322  pts/1  0:00 ps
 		10052 539114  pts/1  0:00 ksh
 		10052 549212  pts/1  0:00 a.out

앞서 구현한 기능으로 위 명령을 수행하면 좀비프로세스가 만들어 질 수 있다.  이는 &명령을 받았을 때 하는 행동의 결과에 따라 좀비프로세스가 될 수도 있고, 정상적으로 종료될 수도 있다. 테스트해본 결과 앞서 구현한 기능으로 위 명령을 수행하면 좀비 프로세스가 만들어지지 않는다. 이유는 자식 프로세스가 execvp를 수행하면서 0, 1, 2번 file descriptor를 close했기 때문에 프롬프트에 자식의 수행결과가 나오지 않으면서 execvp가 정상적으로 끝나면 아무것도 반환하지 않고 종료되기 때문에 zombie 프로세스가 되지 않는다. 만약 자식 프로세스가 Exec를 하지 않게 구현하면서 부모가 wait()을 하지 않으면 좀비 프로세스가 만들어진다.

#### 4.2	5번 테스트의 문제점이 무엇인지 지적하고 해결하기 위한 방법을 제시하라
mysh> sleep 10 &
mysh> sleep 20 &
mysh> ps
mysh> (백그라운드를 기다라지 않고 바로 쉘프롬프트가 출력되는가?)

background를 기다리지 않고 바로 쉘 프롬프트가 출력된다. Background에서 수행되도록 하는 명령인 &를 입력했기 때문에 쉘 프롬프트가 바로 출력되어야 한다. 앞선 4.1번에서 설명한 것처럼 자식 프로세스가 정상적으로 background에서 수행된다면 5번 테스트는 문제없이 동작한다.
 
## 프로젝트 실행 결과

### 5.	실행 결과 
#### 5.1	Cd 
 ![1번](https://user-images.githubusercontent.com/44011462/86113174-dfaa5700-bb03-11ea-8910-67fd88671ea2.gif)

#### 5.2	Exit
 ![2번](https://user-images.githubusercontent.com/44011462/86113191-e769fb80-bb03-11ea-9843-dc460bfe1bcf.gif)

#### 5.3	background & foreground  
 ![3번](https://user-images.githubusercontent.com/44011462/86113192-e8029200-bb03-11ea-8ea5-687d8d3e6099.gif)

#### 5.4	background & foreground 구현 이후에 다음을 테스트 
  

#### 5.5	백그라운드 기다리는 문제 테스트
 ![5번](https://user-images.githubusercontent.com/44011462/86113803-af16ed00-bb04-11ea-8611-0a7d7b0f89f6.gif)

 
#### 5.6	SIGCHLD로 자식 프로세스 wait()시 포그라운드 프로세스가 온전하게 수행되는 테스트
 
#### 5.7	^C ^\ 무시 
 ![7번](https://user-images.githubusercontent.com/44011462/86113716-94447880-bb04-11ea-9ea4-748d20609de3.gif)
#### 5.8	포그라운드 프로세스 ^C 확인
  ![8번](https://user-images.githubusercontent.com/44011462/86113720-960e3c00-bb04-11ea-9a5e-ae15107be9eb.gif)
 
#### 5.9	Redirection
 ![9번](https://user-images.githubusercontent.com/44011462/86113721-96a6d280-bb04-11ea-9260-6072f5921973.gif)
#### 5.10	Pipe
 
#### 5.11	redirection background
 
![11번](https://user-images.githubusercontent.com/44011462/86113724-97d7ff80-bb04-11ea-88c2-e4dea007cc4f.gif)
 
## 프로젝트 전체 소스 코드
 
### 6.	전체 소스 코드
```c
/*
 * 2019.11.18
 * 유닉스 프로그래밍 shell 설계 과제
 * 12164720 정경하 컴퓨터공학과
 * mysh ver3.1
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_CMD_ARG 10
#define MAX_CMD_LIST 10
#define MAX_CMD_GRP 10
#define BUFSIZE 512

const char *prompt = "mysh> ";

char *cmdgrp[MAX_CMD_GRP];
char *cmdlist[MAX_CMD_LIST];
char *cmdargs[MAX_CMD_ARG];
char cmdline[BUFSIZE];

int cmd_cd(int argc, char *argv[]);                                         //디렉토리 변경 함수
int cmd_exit(int argc, char *argv[]);                                       //myshell 종료
void fatal(char *str);                                                      //error 내용을 보여주고 종료하는 함수
int makeargv(char *s, const char *delimiters, char *argvp[], int MAX_LIST); //1차원 배열형태로 받은 cmd를 2차원 배열 형태로 바꾸는 함수
void zombie_handler(int sig);                                               //좀비 프로세스 핸들러
void parse_redirect(char *cmd);                                             //리다이렉션이 필요한지 확인하고 해당 입출력을 바꿔주는 함수
void execute_cmd(char *cmdlist);                                            //명령을 execvp하는 함수
void execute_cmdline(char *cmdline);                                        //하나의 명령 그룹을 execute_cmpgrp에 넣는 함수
void execute_cmdgrp(char *cmdgrp);                                          //여러개의 명령을 각각 execute_cmd에 넣는 함수.

struct sigaction act; //시그널 핸들을 위한 sigaction
static int status;
static int IS_BACKGROUND = 0; //여러 함수들이 백그라운드 실행 여부를 확인하기 위해 전역변수 선언

typedef struct
{ // 이미 결정된 명령을 담을 커맨드 구조체
  char *name;
  char *desc;
  int (*func)(int argc, char *argv[]); // 함수포인터. 사용할 함수들의 매개변수를 맞춰줌
} COMMAND;

static COMMAND builtin_cmds[] = //해당 명령들은 fork되기 전에 수행되어야 하므로 미리 선언하여 실행에 참조
    {
        {"exit", "exit myshell", cmd_exit},
        {"cd", "change directory", cmd_cd},
};

int main(int argc, char *argv[])
{
  int i;
  sigset_t set;

  sigfillset(&set);
  sigdelset(&set, SIGCHLD);
  sigprocmask(SIG_SETMASK, &set, NULL);

  act.sa_flags = SA_RESTART;
  sigemptyset(&act.sa_mask);
  act.sa_handler = zombie_handler; //좀비 프로세스 방지용 핸들러
  sigaction(SIGCHLD, &act, 0);     //자식의 시그널을 받아 미리 지정한 좀비 핸들러를 사용

  while (1)
  {
    fputs(prompt, stdout);               //프롬프트에 mysh> 을 출력
    fgets(cmdline, BUFSIZE, stdin);      //사용자로부터 표준입력을 받음
    cmdline[strlen(cmdline) - 1] = '\0'; //마지막에 NULL을 넣어 EOF를 표시

    execute_cmdline(cmdline); //cmd를 실시하는 함수
  }
  return 0;
}

void fatal(char *message) //에러를 출력하고 종료
{
  perror(message);
  exit(1);
}

int cmd_cd(int argc, char *argv[]) //cd : change directory
{
  if (argc == 1) //홈디렉토리로 이동
    chdir(getenv("HOME"));
  else if (argc == 2)
  {
    if (chdir(argv[1]))              //cd를 하는 시스템콜을 사용
      printf("No such directory\n"); //해당경로가 없는 경우 관련 경고를 출력
  }

  return 1;
}

int cmd_exit(int argc, char *argv[]) //myshell 종료
{
  exit(0);
  return 1;
}

int makeargv(char *s, const char *delimiters, char *argvp[], int MAX_LIST)
{ //s(사용자로부터 입력받은 cmd)를 delimiter를 기준으로 나누어 argvp에 2차원 배열 행태로 삽입 argvp는 주소값의 형태이므로 다른 함수가 참조 가능
  int i = 0, numtokens = 0;
  char *snew = NULL;

  if ((s == NULL) || (delimiters == NULL)) //사용자로부터의 입력이 없거나 파싱 기준이 없는 경우 에러, -1 반환
    return -1;

  snew = s + strspn(s, delimiters);
  argvp[numtokens] = strtok(snew, delimiters);

  if (argvp[numtokens] != NULL)
    for (numtokens = 1; (argvp[numtokens] = strtok(NULL, delimiters)) != NULL; numtokens++)
    {
      if (numtokens == (MAX_LIST - 1)) //EOF를 넣을 공간이 없을 경우 에러, -1 반환
        return -1;
    }

  if (numtokens > MAX_LIST) //너무 많은 입력이 주어진 경우 에러, -1 반환
    return -1;

  return numtokens;
}

void zombie_handler(int sig)
{
  while (waitpid(-1, NULL, WNOHANG) > 0) //자식 프로세스가 좀비 프로세스가 되지 않도록 상태를 확인하여 회수
  {
  }
}

void parse_redirect(char *cmd)
{
  char *arg;
  int fd, i, cmdlen = strlen(cmd);

  for (i = cmdlen - 1; i >= 0; --i) //사용자로 부터 입력받은 cmd를 탐색하여 redirection이 있는지 확인 후 적절한 행동 실시
  {
    switch (cmd[i])
    {
    case '<': //STDIN을 사용자로부터 요청받은 파일로 redirection 하는 곳
      arg = strtok(&cmd[i + 1], " \t");
      if ((fd = open(arg, O_RDONLY | O_CREAT, 0644)) < 0)
        fatal("file open error");
      dup2(fd, STDIN_FILENO); //파일 디스크립터 fd에 표준입력(STDIN_FILENO)을 overwrite
      close(fd);
      cmd[i] = '\0'; //execute에서 '<' 를 알아보지 못하므로 NULL로 바꾸어 cmd를 개량
      break;
    case '>': //사용자로부터 요청받은 파일을 STDOUT로 redirection 하는 곳
      arg = strtok(&cmd[i + 1], " \t");
      if ((fd = open(arg, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
        fatal("file open error");
      dup2(fd, STDOUT_FILENO); //파일 디스크립터 fd에 표준출력(STDOUT_FILENO)을 overwrite
      close(fd);
      cmd[i] = '\0'; //execute에서 '>' 를 알아보지 못하므로 NULL로 바꾸어 cmd를 개량
      break;
    }
  }
}

int parse_background(char *cmd)
{
  int i, type = 0;
  for (i = 0; i < strlen(cmd); ++i)
    if (cmd[i] == '&') //백그라운드 실행을 의미하는 &를 만나는 경우
    {
      cmd[i] = ' '; //execute에서 '>' 를 알아보지 못하므로 white space로 바꿈
      type = 1;     //백그라운드 실행을 뜻하는 1로 반환값을 설정
    }
  return type;
}

void execute_cmd(char *cmdlist)
{
  parse_redirect(cmdlist); //redirection이 필요한지 검사하여 알맞은 행동을 취함

  if (makeargv(cmdlist, " \t", cmdargs, MAX_CMD_ARG) <= 0) //cmd 파싱을 실패한 경우 헤당 에러 메세지 출력 후 종료
    fatal("makeargv_cmdargs error");

  execvp(cmdargs[0], cmdargs); //makeargv에서 만든 명령어를 실행(system call)
  fatal("exec error");         //exev에서 빠져나온다면 문제가 생겼음을 의미하므로 에러처리
}

void execute_cmdgrp(char *cmdgrp)
{
  int i = 0;
  int count = 0;
  int fd[2];
  sigset_t set;

  setpgid(0, 0);
  if (!IS_BACKGROUND)
    tcsetpgrp(STDIN_FILENO, getpid());

  sigfillset(&set);
  sigprocmask(SIG_UNBLOCK, &set, NULL);

  if ((count = makeargv(cmdgrp, "|", cmdlist, MAX_CMD_LIST)) <= 0) //cmd 파싱을 실패한 경우 헤당 에러 메세지 출력 후 종료
    fatal("makeargv_cmdgrp error");

  for (i = 0; i < count - 1; i++) //파이프를 사용하는 cmd에 파이프를 연결
  {
    pipe(fd);
    switch (fork())
    {
    case -1: //fork()의 결과 -1인 경우는 에러. 해당 메세지를 출력 후 종료
      fatal("fork error");
    case 0:
      close(fd[0]);               //사용하지 않는 pipe를 닫음
      dup2(fd[1], STDOUT_FILENO); //표준출력을 pipe fd[1]에 연결
      execute_cmd(cmdlist[i]);
    default:
      close(fd[1]);              //사용하지 않는 pipe를 닫음
      dup2(fd[0], STDIN_FILENO); //표준입력을 pipe fd[0]에 연결
    }
  }
  execute_cmd(cmdlist[i]);
}

void execute_cmdline(char *cmdline)
{
  int count = 0;
  int i = 0, j = 0, pid;
  char *cmdvector[MAX_CMD_ARG];
  char cmdgrptemp[BUFSIZE];
  int numtokens = 0;

  count = makeargv(cmdline, ";", cmdgrp, MAX_CMD_GRP); //수행해야할 cmd의 개수를 계수

  for (i = 0; i < count; ++i)
  {
    memcpy(cmdgrptemp, cmdgrp[i], strlen(cmdgrp[i]) + 1);
    numtokens = makeargv(cmdgrp[i], " \t", cmdvector, MAX_CMD_GRP);

    for (j = 0; j < sizeof(builtin_cmds) / sizeof(COMMAND); ++j)
    {
      if (!strcmp(builtin_cmds[j].name, cmdvector[0])) //이미 선언해둔 exit, cd인 경우에는 fork하기전에 실행
      {
        builtin_cmds[j].func(numtokens, cmdvector);
        return;
      }
    }

    IS_BACKGROUND = parse_background(cmdgrptemp); //백그라운드 실행인지 확인하여 값을 설정 (백그라운드 : 1, 포어그라운드 : 0)

    switch (pid = fork()) //자식 프로세스를 생성하여 알맞은 행동을 실행
    {
    case -1:
      fatal("fork error"); //자식 프로세스를 정상적으로 만들지 못한경우 에러 처리
    case 0:
      execute_cmdgrp(cmdgrptemp); //자식 프로세스가 명령을 수행
    default:
      if (IS_BACKGROUND) //자식 프로세스의 수행이 백그라운드 수행이라면 쉘(부모프로세스)은 기다리지 않음
        break;
      waitpid(pid, NULL, 0);               //자식 프로세스의 수행이 포어그라운드이므로 쉘(부모프로세스)은 작업을 수행중인 자식을 기다림
      tcsetpgrp(STDIN_FILENO, getpgid(0)); //표준 입력에 대한 소유권을 회수
      fflush(stdout);                      //버퍼를 비워냄
    }
  }
}

```