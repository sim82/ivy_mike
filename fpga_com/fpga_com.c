/*
 * Copyright (C) 2011 Simon A. Berger
 * 
 *  This program is free software; you may redistribute it and/or modify its
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 */


#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fpga_com.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static void die_perror( const char *call ) {
    perror( call );
    exit(-1);
}


void fpga_con_init( fpga_con_t *con, const void *daddr, int lport, int dport ) {
    

    
    /*set up local socket address*/
    memset( &con->l_sockaddr, 0, sizeof( con->l_sockaddr ));
    con->l_sockaddr.sin_family = AF_INET;
    con->l_sockaddr.sin_addr.s_addr = INADDR_ANY;
    con->l_sockaddr.sin_port = htons(lport);
    
    /*setup dest socket address*/
    memset( &con->d_sockaddr, 0, sizeof( con->d_sockaddr ));
    con->d_sockaddr.sin_family = AF_INET;
    con->d_sockaddr.sin_addr.s_addr = inet_addr( daddr );
    con->d_sockaddr.sin_port = htons(dport);
    
/*    create my socket*/
    con->s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( con->s < 0 ) {
	die_perror( "socket" );
    }
 
/*    bind it to local socket*/
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


/*send an empty packet for fpga lut initialization*/
void fpga_con_send_init_packet( fpga_con_t *con ) {
    char buf[4];
    
    fpga_con_send( con, buf, 0);
}

/*blocking receive: this call will block until a packet is received*/
ssize_t fpga_con_block_recv( fpga_con_t *con, void *dbuf, size_t dsize ) {
        
    ssize_t rrecv = recv( con->s, dbuf, dsize, 0 );
    
    if( rrecv < 0 ) {
	die_perror( "recv" );
    }
    
    return rrecv;
}


static __inline size_t mymin( size_t a, size_t b ) {
    return a < b ? a : b;
}

const static size_t MTU = 1500;

int fpga_con_send_charv( fpga_con_t *con, char *buf, size_t n ) {
    
    
    const size_t blocksize = MTU;
    
    
    size_t sent = 0;
    while( sent < n ) {
        const size_t to_copy = mymin( blocksize, n - sent );
        
        fpga_con_send( con, (void*) &buf[sent], to_copy );
        
        sent += to_copy;
        
    }
    
    return 1;
    
}

int fpga_con_send_shortv( fpga_con_t *con, short *buf, size_t n ) {
    
    
    const size_t TSIZE = sizeof( short );
    
    const size_t blocksize = MTU / TSIZE;
    
    
    size_t sent = 0;
    while( sent < n ) {
        const size_t to_copy = mymin( blocksize, n - sent );
        
        fpga_con_send( con, (void*) &buf[sent], to_copy * TSIZE );
        
        sent += to_copy;
        
    }
    return 1;
    
}
int fpga_con_send_intv( fpga_con_t *con, int *buf, size_t n ) {
    
    
    const size_t TSIZE = sizeof( int );
    
    const size_t blocksize = MTU / TSIZE;
    
    
    size_t sent = 0;
    while( sent < n ) {
        const size_t to_copy = mymin( blocksize, n - sent );
        
        fpga_con_send( con, (void*) &buf[sent], to_copy * TSIZE );
        
        sent += to_copy;
        
    }
    
    return 1;
}

int fpga_con_send_floatv( fpga_con_t *con, float *buf, size_t n ) {
    return fpga_con_send_intv( con, (int*)buf, n );
}

int fpga_con_send_doublev( fpga_con_t *con, double *buf, size_t n ) {
    
    
    const size_t TSIZE = sizeof( double );
    
    const size_t blocksize = MTU / TSIZE;
    
    
    size_t sent = 0;
    while( sent < n ) {
        const size_t to_copy = mymin( blocksize, n - sent );
        
        fpga_con_send( con, (void*) &buf[sent], to_copy * TSIZE );
        
        sent += to_copy;
        
    }
    
    return 1;
}


