// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <errno.h>

#include "utilities.h"
#include "passivesock.h"
#include "operations.h"


#define QLEN        1		// maximum connections count allowed
#define BUFSIZE     4096	// buffer size
#define EMLEN       20		// expression member (number or operator) initial length
#define SBUFSIZE    64		// send buffer size


struct con_arg {
    pthread_mutex_t *p_mutex;
    int *p_socket;
    int *p_con_count;
};

void finalize(int);
void *processClient(void *args);
void readBuf(int desc);

pthread_mutex_t mutex;

int main(int argc, char *argv[]) {
    char *service = "2241";	// service port number or name
    struct sockaddr_in fsin;	//  client address
    unsigned int alen;		// client address length
    int msock;			// servers main socket
    int ssock;			// servers work socket

    // processing comand line arguments
    switch (argc) {

    case 1:
	break;
    case 2:
	service = argv[1];
	break;
    default:
	errexit("Usage: math-server [port]\n");

    }

    msock = passiveTCP(service, QLEN);
    printf("Server opened on: %s\n", service);

    int con_count = 0;
    pthread_mutex_init(&mutex, NULL);
    signal(SIGTERM, finalize);
    while (1) {

	alen = sizeof(fsin);

	ssock = accept(msock, (struct sockaddr *) &fsin, &alen);

	if (ssock < 0) {

	    if (errno == EINTR)
		continue;

	    errexit("accept: %s\n", strerror(errno));

	}

	pthread_t tid;
	pthread_mutex_lock(&mutex);
	if (con_count >= QLEN) {
	    close(ssock);
	    pthread_mutex_unlock(&mutex);
	    continue;
	}
	++con_count;
	pthread_mutex_unlock(&mutex);
	struct con_arg arg = {.p_mutex = &mutex,.p_socket =
		&ssock,.p_con_count = &con_count
	};
	pthread_create(&tid, NULL, &processClient, &arg);
	pthread_detach(tid);
    }

    return 0;
}

// Removes processes from system tables
void finalize(int sig) {
    pthread_mutex_destroy(&mutex);
}

void *processClient(void *args) {
    struct con_arg *arg = (struct con_arg *) args;
    pthread_mutex_t *p_mutex = arg->p_mutex;
    int socket = *arg->p_socket;
    int *p_con_count = arg->p_con_count;
    // body
    readBuf(socket);
    // body end
    pthread_mutex_lock(p_mutex);
    --*p_con_count;
    pthread_mutex_unlock(p_mutex);
    close(socket);
    pthread_exit(NULL);
}

int is_number(unsigned char ch) {
    return isdigit(ch) || ch == '-';
}

int is_digit(unsigned char ch) {
    return isdigit(ch);
}

int is_operation(unsigned char ch) {
    return !isdigit(ch) && ch != ' ';
}

void look_exp_member(int (*condition_f)(unsigned char), unsigned char ch,
		     char **exp_member_str, int *exp_mem_len, int *symbc,
		     int *wphase) {
    if( (*condition_f)(ch)) {
	*symbc = 1;
	*exp_mem_len = EMLEN;
	*exp_member_str = (char *) malloc(*exp_mem_len * sizeof(char));
	(*exp_member_str)[0] = ch;
	*wphase = 1;
    }
}

void read_exp_member(int (*condition_f)(unsigned char), unsigned char ch,
		     char **exp_member_str, int *exp_mem_len, int *symbc,
		     int *wphase, int wphase_val, int *iter) {
    ++*symbc;
    if (*symbc > *exp_mem_len - 1) {
	*exp_mem_len += EMLEN;
	char *tmp =
	    (char *) realloc(*exp_member_str, *exp_mem_len * sizeof(char));
	if (tmp == NULL) {
	    free(*exp_member_str);
	    errexit("realoc failed");
	    return;
	}
	*exp_member_str = tmp;
    }
    if ((*condition_f) (ch)) {
	(*exp_member_str)[(*symbc) - 1] = ch;
    } else {
	--*iter;
	(*exp_member_str)[(*symbc) - 1] = '\0';
	*wphase = wphase_val;
    }
}

int process_int32(char *number_str, int32_t * num, char **answer) {
    long int l_number = strtol(number_str, NULL, 10);
    if (errno == ERANGE || l_number < INT32_MIN || l_number > INT32_MAX) {
	*answer =
	    (char *) malloc((strlen("INT32 overflow") + 1) * sizeof(char));
	strcpy(*answer, "INT32 overflow");
	return 0;
    }
    *num = (int32_t) l_number;
    return 1;
}

void readBuf(int desc) {
    char buf[BUFSIZE];		//  Буфер приёма
    char sbuf[SBUFSIZE];	//  Буфер отправки
    int rc, sc;			//  Количество считанных и записываемых байт
    int symbc;			//  Количество символов в подпоследовательности
    int exp_mem_len;
    int wphase = 0;		//  Фаза работы обрабатывающего процесса
    // at 0 - first number
    // at 1 - second number
    // at 2 - operator
    char *exp_members[3] = { NULL, NULL, NULL };
    int is_ending_sequence = 0;
    char *answer;

    // Set connection timeout
    struct timeval timeout;
    // 15 seconds
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;
    if (setsockopt
	(desc, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
	 sizeof(timeout)) < 0) {
	errexit("can't set socket options");
    }

    while (desc >= 0 && (rc = recv(desc, buf, BUFSIZE, 0))) {

	if (rc < 0) {
	    // Socket timeout
	    if (errno == EAGAIN)
		break;
	    errexit("subseq read: %s\n", errno, strerror(errno));
	}

	for (int i = 0; i < rc; ++i) {
	    if (buf[i] < 0) {
		return;
		//errexit("have negative char in the read buffer\n");
	    }

	    unsigned char ch = buf[i];
	    switch (wphase) {

		// looking for expression member
	    case 0:

		// it's first number case
		if (exp_members[0] == NULL) {
		    look_exp_member(is_number, ch, &exp_members[0],
				    &exp_mem_len, &symbc, &wphase);
		    // it's operation case
		} else if (exp_members[2] == NULL) {
		    look_exp_member(is_operation, ch, &exp_members[2],
				    &exp_mem_len, &symbc, &wphase);
		    // it's second number case
		} else {
		    look_exp_member(is_number, ch, &exp_members[1],
				    &exp_mem_len, &symbc, &wphase);
		}
		break;
		// reading the expression member
	    case 1:
		// it's first number case
		if (exp_members[1] == NULL && exp_members[2] == NULL) {
		    read_exp_member(is_digit, ch, &exp_members[0],
				    &exp_mem_len, &symbc, &wphase, 0, &i);
		    // it's second number case
		} else if (exp_members[0] != NULL && exp_members[1] != NULL
			   && exp_members[2] != NULL) {
		    read_exp_member(is_digit, ch, &exp_members[1],
				    &exp_mem_len, &symbc, &wphase, 2, &i);
		    // it's operation case
		} else {
		    read_exp_member(is_operation, ch, &exp_members[2],
				    &exp_mem_len, &symbc, &wphase, 0, &i);
		}
		break;
		// looking for end of the message (\r\n)
	    case 2:
		// we found \r\n. calculate the stuff and send answer
		if (is_ending_sequence && ch == '\n') {
		    int32_t first_number, second_number;
		    if (process_int32
			(exp_members[0], &first_number, &answer)
			&& process_int32(exp_members[1], &second_number,
					 &answer)) {
			int64_t result;
			enum e_calculation_result calc_result =
			    performCalculation(exp_members[2],
					       first_number, second_number,
					       &result);
			if (calc_result == ecrUnknownOperation) {
			    answer = (char *)
				malloc((strlen("unknown operation") +
					1) * sizeof(char));
			    if (answer == NULL) {
				errexit("malloc failed");
				return;
			    }
			    strcpy(answer, "unknown operation");
			} else if (calc_result == ecrArithmeticError) {
			    answer = (char *)
				malloc((strlen
					("arithmetic error (are you dividing by zero?)")
					+ 1) * sizeof(char));
			    if (answer == NULL) {
				errexit("malloc failed");
				return;
			    }
			    strcpy(answer,
				   "arithmetic error (are you dividing by zero?)");
			} else if (calc_result == ecrInt32Overflow) {
			    answer = (char *)
				malloc((strlen("INT32 overflow") +
					1) * sizeof(char));
			    if (answer == NULL) {
				errexit("malloc failed");
				return;
			    }
			    strcpy(answer, "INT32 overflow");
			} else {
			    char bu[sizeof(char) * (11 + 1)];
			    i32toa((int32_t) result, bu);
			    answer =
				(char *) malloc((strlen(bu) + 1) *
						sizeof(char));
			    if (answer == NULL) {
				errexit("malloc failed");
				return;
			    }
			    strcpy(answer, bu);
			}	// else {
//                            errexit("unknown value of type enum e_calculation_result");
//                        }
		    }
		    for (int j = 0; j < 3; ++j) {
			free(exp_members[j]);
			exp_members[j] = NULL;
		    }
		    // answer is ready. let's send
		    sc = sprintf(sbuf, "%s\r\n", answer);
		    if (write(desc, sbuf, sc) < 0)
			errexit("subseq write: %s\n", strerror(errno));
		    // freeing memory
		    free(answer);
		    is_ending_sequence = 0;

		    wphase = 0;

		    close(desc);
		    desc = -1;
		    break;
		}
		is_ending_sequence = ch == '\r';
		break;

	    default:

		errexit("invalid work phase: %s\n", wphase);

	    }

	}

    }

    // if connection was shutdown before parsing
    for (int j = 0; j < 3; ++j) {
	if (exp_members[j] != NULL)
	    free(exp_members[j]);
    }
}
