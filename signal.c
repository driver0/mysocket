#include <signal.h>
#include "cli_serv.h"

Sigfunc *signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (signo == SIGALRM) {
#ifdef 	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif 
	} else {
#ifdef 	SA_RESTART	/* Restart syscall on signal return. */
		act.sa_flags |= SA_RESTART;
#endif 
	}

	if (sigaction(signo, &act, &oact) == -1) {
		return SIG_ERR;
	}
	return oact.sa_handler;
}
