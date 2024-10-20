#include <cstring>
//#include "port_common.h"

#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"




/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x00, 0x00, 0x00}, // MAC address
        .ip = {192, 168, 1, 6},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 1, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};



void InitEthernet(void)
{
    wizchip_spi_initialize();
    wizchip_cris_initialize();
    wizchip_reset();

    wizchip_initialize();
    wizchip_check();

    network_initialize(g_net_info);

    /* Get network information */
    print_network_information(g_net_info);
}







