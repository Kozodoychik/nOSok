#include <stdint.h>

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
				}__attribute__((packed)) bootp_message;

				typedef struct {
					uint8_t* buffer;
					unsigned int offset;
				} dhcp_option_builder;

				typedef struct {
					uint8_t code;
					uint8_t length;
					uint8_t* data;
				} dhcp_option;

				void add_cookie(dhcp_option_builder* b);
				void add_option(dhcp_option_builder* b, uint8_t code, uint8_t length, uint8_t* data);
				void add_client_id(dhcp_option_builder* b, uint8_t hw_type, uint8_t* addr);
				void add_end(dhcp_option_builder* b);

				dhcp_option read_option(uint8_t* buffer, unsigned int offset);

				void init();

				void do_discover();
				void do_request(uint8_t ip[4]);

			}
		}
	}
}