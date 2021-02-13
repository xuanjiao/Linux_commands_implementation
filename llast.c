#include	<stdio.h>
#include    <fcntl.h>
#include	<sys/types.h>
#include	<utmp.h>
#include 	<time.h>
#include 	<string.h>
#include 	<unistd.h>
#include 	<stdlib.h>

#define NRECS 5
#define TTYSIZE 5
#define UTSIZE (sizeof(struct utmp))
//#define WTMP_FILE "/var/log/wtmp"


static char wtmp_buf[NRECS*UTSIZE];
static int wtmp_fd = -1;
static char terminal[TTYSIZE];
static int t_flag = 0;

int wtmp_open(){
	wtmp_fd = open(WTMP_FILE,O_RDONLY);
	return wtmp_fd;
}

int wtmp_reload(){
/*
 *      read next bunch of records into buffer
 */
	int byte_read,rec_read;
	
	memset(wtmp_buf,0,NRECS*UTSIZE);
	byte_read = read(wtmp_fd,wtmp_buf,NRECS * UTSIZE); /* read data	*/
	if(byte_read <0){	/* error?*/
		printf("read finish");
		return -1;
	}

	rec_read = byte_read / UTSIZE;
	return rec_read;
}

void print_buf_reverse(int rec_read){
	struct utmp	*rp;
	char	*typename();

	
	rp = (struct utmp*)&wtmp_buf[(NRECS-1)*UTSIZE]; /* get address of the last record in buffer   */
	for(int i=0; i < rec_read;i++){
	//	if(rp->ut_type == USER_PROCESS || rp->ut_type == BOOT_TIME  ){
	
		if(t_flag==0 ||strcmp(terminal,rp->ut_line)==0){
			
			if(strcmp(rp->ut_host,":0")==0){
				printf("%-8.8s ", rp->ut_user );
				printf("%-12.12s ", strcmp(rp->ut_line,"~")==0?"system boot":rp->ut_line);
				printf("%-12.12s ", rp->ut_host);
				printf("%4d %-14.14s ", rp->ut_type , typename(rp->ut_type) );
				printf("%16.16s ",ctime(&rp->ut_time));
				
				putchar('\n'); 
				rp--;
			}
		}
	}
}

int wtmp_seek(int num_recs){
	lseek(wtmp_fd,(-1)*num_recs*UTSIZE,SEEK_CUR);
}

int wtmp_close(){
	int ret = 0;
	if(wtmp_fd != -1)
		ret = close(wtmp_fd);
	return wtmp_fd;
}

char *uttypes[] = {  	"EMPTY", "RUN_LVL", "BOOT_TIME", "OLD_TIME", 
			"NEW_TIME", "INIT_PROCESS", "LOGIN_PROCESS", 
			"USER_PROCESS", "DEAD_PROCESS", "ACCOUNTING"
	};

char *
typename( int typenum )
{
	return uttypes[typenum];
}

int main(int argc, char* argv[]){
	int rec_read = -2;
	
	
	if(argc == 2){
		t_flag = 1;
		strcpy(terminal,argv[1]);
		printf("Terminal name: %s\n",terminal);
	}
	else if(argc !=1 && argc !=2){
		printf("Input: llast ot llast [Terminal name]\n");
		exit(0);
	}
	

	if( wtmp_open()==-1){
		perror("Can not open file"); /* error?*/
		exit(1);
	}
	
	if(lseek(wtmp_fd,(-1)*NRECS*UTSIZE,SEEK_END)<0){
		perror("failed to move file pointer\n");
		exit(1);
	}
	while ((rec_read = wtmp_reload())>0){
		print_buf_reverse(rec_read);
		
		if(lseek(wtmp_fd,(-2)*rec_read*UTSIZE,SEEK_CUR)<0){
			//perror("failed to move file pointer");
			exit(0);
		}
	}
	
	wtmp_close();
	
}


