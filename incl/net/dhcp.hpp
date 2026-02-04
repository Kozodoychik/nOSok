#include <stdint.h>

#define DHCP_COOKIE		0x63538263

#define DHCP_OPTION_MSG_TYPE		53
#define DHCP_OPTION_CLIENT_ID		61
#define DHCP_OPTION_PARAMETER_LIST	55

#define DHCP_OPTION_END				0xff

namespace nosok {
	namespace net {
		namespace ip {
			namespace dhcp {

				typedef struct {
					uint8_t			op;
					uint8_t			hw_type;
					uint8_t			hw_len;
					uint8_t			hops;
					uint32_t		xid;
					uint16_t		secs;
					uint16_t		flags;
					uint8_t			ciaddr[4];
					uint8_t			yiaddr[4];
					uint8_t			siaddr[4];
					uint8_t			giaddr[4];
					uint8_t			client_mac[6];
					uint8_t			padding[10];
					char			sname[64];
					char			boot_file[128];
					uint32_t		dhcp_cookie;
					uint8_t			vend[64];
				}__attribute__((packed)) dhcp_message;

				void init();

			}
		}
	}
}