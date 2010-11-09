#include <arpa/inet.h>


#ifdef __cplusplus
extern "C" {
#endif
    

typedef struct {
    // local and remote socket addresses
    struct sockaddr_in l_sockaddr;
    struct sockaddr_in d_sockaddr;
    
    // my socket
    int s;
    
} fpga_con_t;


void fpga_con_init( fpga_con_t *con, const void *daddr, int lport, int dport );
ssize_t fpga_con_send( fpga_con_t *con, const void *buf, size_t len );
void fpga_con_send_init_packet( fpga_con_t *con );
ssize_t fpga_con_block_recv( fpga_con_t *con, void *dbuf, size_t dsize );



#ifdef __cplusplus
}
#endif
