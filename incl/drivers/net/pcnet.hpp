#include <stdint.h>
#include <drivers/pci.hpp>

#define PCNET_16_RDP	0x10
#define PCNET_16_RAP	0x12
#define PCNET_16_RESET	0x14
#define PCNET_16_BDP	0x16

#define PCNET_32_RDP	0x10
#define PCNET_32_RAP	0x14
#define PCNET_32_RESET	0x18
#define PCNET_32_BDP	0x1c

typedef struct {
	uint32_t	rx_buffer;
	uint16_t	bcnt		: 12;
	uint8_t		ones		: 4;
	uint8_t		reserved;
	bool		enp			: 1;
	bool		stp			: 1;
	bool		buff		: 1;
	bool		crc			: 1;
	bool		oflo		: 1;
	bool		fram		: 1;
	bool		err			: 1;
	bool		own			: 1;
	uint16_t	mcnt		: 12;
	uint8_t		zeroes		: 4;
	uint8_t		rpc;
	uint8_t		rcc;
	uint32_t	reserved2;
}__attribute__((packed)) pcnet_rx_descr;

typedef struct {
	uint32_t	tx_buffer;
	uint16_t	bcnt		: 12;
	uint8_t		ones		: 4;
	uint8_t		reserved;
	bool		enp			: 1;
	bool		stp			: 1;
	bool		def			: 1;
	bool		one			: 1;
	bool		more		: 1;
	bool		add_no_fcs	: 1;
	bool		err			: 1;
	bool		own			: 1;
	uint8_t		trc			: 4;
	uint16_t	reserved2	: 12;
	uint16_t	tdr			: 10;
	bool		retry		: 1;
	bool		lcar		: 1;
	bool		lcol		: 1;
	bool		ex_def		: 1;
	bool		uflo		: 1;
	bool		buff		: 1;
	uint32_t	reserved3;
}__attribute__((packed)) pcnet_tx_descr;

typedef struct {
	uint16_t		mode;
	uint8_t			reserved	: 4;
	uint8_t			rlen		: 4;
	uint8_t			reserved2	: 4;
	uint8_t			tlen		: 4;
	uint8_t			mac[6];
	uint16_t		reserved3;
	uint32_t		ladr_lo;
	uint32_t		ladr_hi;
	uint32_t		rdra;
	uint32_t		tdra;
}__attribute__((packed)) pcnet_init_block;

using namespace nosok::devices::pci;

class PCNET: public PCINetworkDevice {
public:

	PCNET(device_info info);
	~PCNET();

	void send_packet(void* buffer, unsigned int size) override;

	void init() override;
	void irq() override;

private:
	bool init_done = false;

	alignas(16) pcnet_rx_descr rx_descr[8];
	alignas(16) pcnet_tx_descr tx_descr[8];

	alignas(16) uint8_t rx_buffer[1536 * 8];
	alignas(16) uint8_t tx_buffer[1536 * 8];

	alignas(16) pcnet_init_block init_block;

	int rx_n = 0;
	int tx_n = 0;

	void write_rap16(uint16_t value);
	void write_rap32(uint32_t value);

	uint16_t read_csr16(uint32_t port);
	uint32_t read_csr32(uint32_t port);

	void write_csr16(uint32_t port, uint16_t value);
	void write_csr32(uint32_t port, uint32_t value);

	uint16_t read_bcr16(uint32_t port);
	uint32_t read_bcr32(uint32_t port);

	void write_bcr16(uint32_t port, uint16_t value);
	void write_bcr32(uint32_t port, uint32_t value);

};