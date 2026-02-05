#include <net/arp.hpp>
#include <net/ethernet.hpp>
#include <std/string.h>
#include <std/inet.hpp>
#include <std/printf.hpp>
#include <memory/alloc.hpp>

namespace nosok {
	namespace net {
		namespace arp {

			arp_cache_entry* cache;
			arp_cache_entry* last_entry;

			void init() {
				cache = 0;
				last_entry = 0;
			}

			void handle_packet(void* buffer) {
				arp_packet* packet = (arp_packet*)buffer;

				arp_cache_entry* entry = get_by_ip(packet->dst_ip);

				if (!entry) return;

				arp_packet response;

				memcpy(response.src_ip, entry->ip, 4);
				memcpy(response.dst_ip, packet->src_ip, 4);
				memcpy(response.src_mac, nosok::net::get_mac(), 6);
				memcpy(response.dst_mac, packet->src_mac, 6);

				response.opcode = htons(2);
				response.hw_type = htons(1);
				response.protocol_type = htons(0x0800);
				response.hw_addr_len = 6;
				response.pr_addr_len = 4;

				ethernet_header_t eth_header;

				eth_header.ether_type = htons(0x0806);
				
				nosok::net::send(eth_header, &response, sizeof(arp_packet));
			}

			void add_cache_entry(uint8_t ip[4], uint8_t mac[6]) {
				arp_cache_entry* entry = new arp_cache_entry;
				entry->next = 0;

				memcpy(entry->ip, ip, 4);
				memcpy(entry->mac, mac, 6);

				if (cache == 0) {
					cache = entry;
					last_entry = cache;
					return;
				}

				last_entry->next = entry;
				last_entry = entry;
			}

			arp_cache_entry* get_by_ip(uint8_t ip[4]) {
				for (arp_cache_entry* entry = cache; entry != 0; entry = entry->next) {
					if (!memcmp(entry->ip, ip, 4)) return entry;
				}
				return 0;
			}

			arp_cache_entry* get_by_mac(uint8_t mac[6]) {
				for (arp_cache_entry* entry = cache; entry != 0; entry = entry->next) {
					if (!memcmp(entry->mac, mac, 6)) return entry;
				}
				return 0;
			}
		}
	}
}