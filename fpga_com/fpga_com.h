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

#include <arpa/inet.h>


#ifdef __cplusplus
extern "C" {
#endif
    

typedef struct {
    /* local and remote socket addresses */
    struct sockaddr_in l_sockaddr;
    struct sockaddr_in d_sockaddr;
    
    /* my socket */
    int s;
    
} fpga_con_t;


void fpga_con_init( fpga_con_t *con, const void *daddr, int lport, int dport );
ssize_t fpga_con_send( fpga_con_t *con, const void *buf, size_t len );
void fpga_con_send_init_packet( fpga_con_t *con );
ssize_t fpga_con_block_recv( fpga_con_t *con, void *dbuf, size_t dsize );



int fpga_con_send_charv( fpga_con_t *con, char *buf, size_t n );
int fpga_con_send_shortv( fpga_con_t *con, short *buf, size_t n );
int fpga_con_send_intv( fpga_con_t *con, int *buf, size_t n );
int fpga_con_send_floatv( fpga_con_t *con, float *buf, size_t n );
int fpga_con_send_doublev( fpga_con_t *con, double *buf, size_t n );


#ifdef __cplusplus
}
#endif
