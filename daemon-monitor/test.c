#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <dlfcn.h>
#include <sched.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#define BRC_SHM_ENV_VAR     "__BRC_SHM_ID"
#define SIZE 500

static int brc_shm_id;

static void remove_shm(void) {  
  shmctl(brc_shm_id, IPC_RMID, NULL);
}

int main() {

    int fd;
    char *myfifo = "/tmp/fifopipe";
    char buf[50];

    mkfifo(myfifo, 0666);

    fd = open(myfifo, O_WRONLY);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    // char *msg;
    // msg="This is the string to be reversed";
    
    // write(fd, msg, strlen(msg)+1);
    // close(fd);

    //


    char* brc_shm_str;
    brc_shm_id = shmget(IPC_PRIVATE, sizeof(char) * SIZE, IPC_CREAT | IPC_EXCL | 0600);
    if (brc_shm_id < 0) printf("%s\n", "shmget() failed");
    atexit(remove_shm);

    brc_shm_str = (char*)malloc(sizeof(char)*10);
    sprintf(brc_shm_str, "%d", brc_shm_id);

    printf("%s\n", brc_shm_str);

    setenv(BRC_SHM_ENV_VAR, brc_shm_str, 1);
    write(fd, brc_shm_str, strlen(brc_shm_str));
    //write(fd, &brc_shm_id, sizeof(brc_shm_id));
    close(fd);
    //unlink(myfifo); 
    free(brc_shm_str);

    char *__aif_untouched_ptr = shmat(brc_shm_id, NULL, 0);

    __aif_untouched_ptr[SIZE-1] = 1;
    while(1) {
      sleep(10);
      //printf("%s\n", getenv(BRC_SHM_ENV_VAR));
    }
}