/* dumputmp.c 
 *
 *	purpose:	display contents of wtmp or utmp in readable form
 *	args:		none for default (/etc/utmp) or a filename
 *	action:		open the file and read it struct by struct,
 *			displaying all members of struct in nice columns
 *	history:	Feb 15 1996: added buffering using utmplib
 *	compiling:	to compile this, use
 *			gcc dumputmp.c utmplib.c -o dumputmp
 */

#include	<stdio.h>
#include	<sys/types.h>
#include	<utmp.h>
#include 	<time.h>
#include 	<string.h>

#define BUF_SIZE 50
struct utmp dump_buf[BUF_SIZE];
int cur_utmp;

main(int ac,char **av)
{
	cur_utmp = BUF_SIZE-1;
	if ( ac == 1 )
		dumpfile( UTMP_FILE );
	else
		dumpfile( av[1] );
}

dumpfile( char *fn )
/*
 * open file and dump records
 */
{
	struct utmp	*utp,		/* ptr to struct	*/
			*utmp_next();	/* declare its type	*/

	if ( utmp_open( fn ) == -1 )	/* open file		*/
	{
		perror( fn );
		return ;
	}

	/* loop, reading records and showing them */

	while( utp = utmp_next() ){
		memcpy(dump_buf+cur_utmp,utp,sizeof(struct utmp));
		printf("cur_utmp = %d\n",cur_utmp);
		if(cur_utmp==0)
			break;
		else
			cur_utmp--;
	}
	utmp_close();
	
	
	for(int i = cur_utmp; i < BUF_SIZE ; i++){
		show_utrec(dump_buf+i);
		
		/* Clear buffer and buffer pointer		*/
		//cur_utmp = 0;		
		//memcpy(dump_buf,0,sizeof(dump_buf));
	}
}
	
show_utrec( struct utmp *rp )
{
	char	*typename();
	long timeval = rp->ut_time;
	char* cp = ctime(&timeval);
	
	printf("%-8.8s ", rp->ut_user );
	printf("%-12.12s ", rp->ut_id   );
	printf("%-12.12s ", rp->ut_line );
	printf("%6d ", rp->ut_pid );
	printf("%4d %-14.14s ", rp->ut_type , typename(rp->ut_type) );
	printf("%12.12s ", cp +4 );
	printf("%s", rp->ut_host );
	putchar('\n');
}

char *uttypes[] = {  	"EMPTY", "RUN_LVL", "BOOT_TIME", "OLD_TIME", 
			"NEW_TIME", "INIT_PROCESS", "LOGIN_PROCESS", 
			"USER_PROCESS", "DEAD_PROCESS", "ACCOUNTING"
	};

char *
typename( typenum )
{
	return uttypes[typenum];
}
