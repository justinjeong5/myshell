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
