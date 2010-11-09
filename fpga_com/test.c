#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fpga_com.h"

//
// compile with: gcc -Wall -std=c99 test.c -o test
//



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