#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    // local and remote socket addresses
    struct sockaddr_in l_sockaddr;
    struct sockaddr_in d_sockaddr;
    
    // my socket
    int s;
    
} fpga_con_t;



void die_perror( const char *call ) {
    perror( call );
    exit(-1);
}

void fpga_con_init( fpga_con_t *con, const void *daddr, int lport, int dport ) {
    
    printf( "size: %zd %zd\n", sizeof( con->l_sockaddr ), sizeof( struct sockaddr_in ));
    
    // set up local socket address
    memset( &con->l_sockaddr, 0, sizeof( con->l_sockaddr ));
    con->l_sockaddr.sin_family = AF_INET;
    con->l_sockaddr.sin_addr.s_addr = INADDR_ANY;
    con->l_sockaddr.sin_port = htons(lport);
    
    // setup dest socket address
    memset( &con->d_sockaddr, 0, sizeof( con->d_sockaddr ));
    con->d_sockaddr.sin_family = AF_INET;
    con->d_sockaddr.sin_addr.s_addr = inet_addr( daddr );
    con->d_sockaddr.sin_port = htons(dport);
    
    // create my socket
    con->s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( con->s < 0 ) {
	die_perror( "socket" );
    }
 
    // bind it to local socket
    int r = bind( con->s, (struct sockaddr *)&con->l_sockaddr, sizeof(con->l_sockaddr));
    if( r < 0 ) {
	die_perror( "bind" );
    }
 
 
}

ssize_t fpga_con_send( fpga_con_t *con, const void *buf, size_t len ) {
	
    ssize_t rsend = sendto( con->s, buf, len, 0, (struct sockaddr*)&con->d_sockaddr, sizeof(con->d_sockaddr));
    if( rsend < 0 ) {
	die_perror( "sendto" );
    }
    
    return rsend;
}


// send an empty packet for fpga lut initialization
void fpga_con_send_init_packet( fpga_con_t *con ) {
    char buf[4];
    
    fpga_con_send( con, buf, 0);
}

// blocking receive: this call will block until a packet is received
ssize_t fpga_con_block_recv( fpga_con_t *con, void *dbuf, size_t dsize ) {
        
    ssize_t rrecv = recv( con->s, dbuf, dsize, 0 );
    
    if( rrecv < 0 ) {
	die_perror( "recv" );
    }
    
    return rrecv;
}


static void swp( char *s1, char *s2 ) {
    char tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}
static void swap_endian( double *dbl ) {
    char *b = (char *)dbl;
    
    for( int i = 0; i < 4; i++ ) {
	swp((char*)&b[i], (char*)&b[7-i]);
    }
}

int main() {
       
    fpga_con_t con;
    
    const char *fpga_addr = "127.0.0.1";
    int fpga_port = 1235;
    int my_port = 1234;
    
    fpga_con_init( &con, fpga_addr, my_port, fpga_port );
    
    
    
    fpga_con_send_init_packet( &con );
    
    const int N_DBL = 16;
    double sdbl[N_DBL];
    
    // fill send buffer with doubles. do endiannes swap before sending
    for( int i = 0; i < N_DBL; i++ ) {
	sdbl[i] = i * 1000.0;
	swap_endian(&sdbl[i]);
    }
    
    fpga_con_send( &con, sdbl, N_DBL * sizeof( double ) );
    
    
    
    double rdbl[1024];
    
    
    ssize_t rsize = fpga_con_block_recv( &con, rdbl, sizeof( rdbl ) );
 
    
    printf( "recv: %zd\n", rsize );
    
    // interpret received char arrays as double array. do endiannes swap before printing
    
    for( int i = 0; i < rsize / sizeof( double ); i++ ) {
	swap_endian( &rdbl[i] );
	printf( "rdbl: %d %f\n", i, rdbl[i] );
    }
    
    
}