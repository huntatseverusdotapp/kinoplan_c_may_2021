#include "passivesock.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utilities.h"

// port number starting point
unsigned short portbase = 0;

int passivesock(const char* service, const char* transport, int qlen)
/*
 *  Parameters:
 *      service     - service name or port number
 *      transport   - protocol name ("udp"/"tcp")
 *      qlen        - query length to limit connections (only tcp)
 */
{
    struct servent *pse;        // service info
    struct protoent *ppe;       // protocol info
    struct sockaddr_in sin;     // IP address
    int s, type;                // socket descriptor and type

    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if ((pse = getservbyname(service, transport)))

        sin.sin_port = htons( ntohs( (unsigned short) pse->s_port ) + portbase );

    else if ( ( sin.sin_port = htons( (unsigned short) atoi(service) ) ) == 0 )

        errexit("can't get \"%s\" service entry\n", service);

    if ( ( ppe = getprotobyname(transport) ) == 0 )

        errexit("can't get \"%s\" protocol entry\n", transport);

    if ( strcmp(transport, "udp") == 0 )

        type = SOCK_DGRAM;

    else

        type = SOCK_STREAM;

    // socket
    s = socket(PF_INET, type, ppe->p_proto);

    if (s < 0)

        errexit("can't create socket: %s\n", strerror(errno));

    // binding socket
    if ( bind( s, (struct sockaddr *) &sin, sizeof(sin) ) < 0 )

        errexit("can't bind to %s port: %s\n", service, strerror(errno));

    if ( type == SOCK_STREAM && listen(s, qlen) )

        errexit("can't listen to %s port: %s\n", service, strerror(errno));

    return s;
}

int passiveUDP(const char *service)
{
    return passivesock(service, "udp", 0);
}

int passiveTCP(const char *service, int qlen)
{
    return passivesock(service, "tcp", qlen);
}

