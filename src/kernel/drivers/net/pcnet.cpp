#include <drivers/net/pcnet.hpp>
#include <drivers/io.hpp>
#include <drivers/pci.hpp>
#include <std/printf.hpp>
#include <std/string.h>
#include <cpu/interrupts.hpp>

void PCNET::irq() {
	uint32_t status = this->read_csr32(0);

	if (status & 0x100) this->init_done = true;

	nosok::io::printf("PCNET IRQ!!!\n");

	this->write_csr32(0, status | 0x400);
}

void PCNET::write_rap16(uint16_t value) {
	nosok::io::ports::write16(this->io_base + PCNET_16_RAP, value);
}

void PCNET::write_rap32(uint32_t value) {
	nosok::io::ports::write32(this->io_base + PCNET_32_RAP, value);
}

// Доступ к CSR (порт RDP)

uint16_t PCNET::read_csr16(uint32_t port) {
	this->write_rap16(port);
	return nosok::io::ports::read16(this->io_base + PCNET_16_RDP);
}

uint32_t PCNET::read_csr32(uint32_t port) {
	this->write_rap32(port);
	return nosok::io::ports::read32(this->io_base + PCNET_32_RDP);
}

void PCNET::write_csr16(uint32_t port, uint16_t value) {
	this->write_rap16(port);
	nosok::io::ports::write16(this->io_base + PCNET_16_RDP, value);
}

void PCNET::write_csr32(uint32_t port, uint32_t value) {
	this->write_rap32(port);
	nosok::io::ports::write32(this->io_base + PCNET_32_RDP, value);
}

// Доступ к BCR (порт BDP)

uint16_t PCNET::read_bcr16(uint32_t port) {
	this->write_rap16(port);
	return nosok::io::ports::read16(this->io_base + PCNET_16_BDP);
}

uint32_t PCNET::read_bcr32(uint32_t port) {
	this->write_rap32(port);
	return nosok::io::ports::read32(this->io_base + PCNET_32_BDP);
}

void PCNET::write_bcr16(uint32_t port, uint16_t value) {
	this->write_rap16(port);
	nosok::io::ports::write16(this->io_base + PCNET_16_BDP, value);
}

void PCNET::write_bcr32(uint32_t port, uint32_t value) {
	this->write_rap32(port);
	nosok::io::ports::write32(this->io_base + PCNET_32_BDP, value);
}

PCNET::PCNET(device_info info) : PCIDevice(info) {};

void PCNET::init() {
	this->info.vendor_id = 0x1022;
	this->info.device_id = 0x2000;

	nosok::devices::pci::device_header header = nosok::devices::pci::get_device_header(this->info);
	this->irq_n = header.irq_line;
	nosok::cpu::interrupts::unmask_irq(header.irq_line);

	nosok::io::printf("AMD PCnet-PCI II detected\n");

	// Включаем Bus mastering
	uint32_t config = nosok::devices::pci::config_read(this->info, 0x04);
	//config &= 0xffff0000;
	config |= 0x05;
	nosok::devices::pci::config_write(this->info, 0x04, config);

	// Получаем начальный адрес портов ввода/вывода
	uint32_t io_base = header.bar0 & 0xfffffffc;
	nosok::io::printf("PCNET I/O base: 0x%x\n", io_base);
	nosok::io::printf("PCNET IRQ: 0x%x\n", header.irq_line);

	this->io_base = io_base;

	// Reset
	nosok::io::ports::read16(this->io_base + PCNET_16_RESET);
	nosok::io::ports::read32(this->io_base + PCNET_32_RESET);

	nosok::io::ports::write32(this->io_base + PCNET_32_RDP, 0);

	// Читаем MAC-адрес
	uint8_t mac[6];

	for (int i = 0; i < 6; i++) {
		mac[i] = (uint8_t)(nosok::io::ports::read32(this->io_base + i) & 0xff);
	}

	nosok::io::printf("MAC: %2x:%2x:%2x:%2x:%2x:%2x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	// SWSTYLE
	uint32_t csr58 = this->read_csr32(58);
	csr58 &= 0xff00;
	csr58 |= 2;
	this->write_csr32(58, csr58);

	// ASEL
	uint32_t bcr2 = this->read_bcr32(2);
	bcr2 |= 2;
	this->write_bcr32(2, bcr2);

	// Подготаливаем дескрипторы и буфферы
	memset(this->rx_descr, 0, sizeof(this->rx_descr));
	memset(this->tx_descr, 0, sizeof(this->tx_descr));

	memset(&this->init_block, 0, sizeof(this->init_block));

	uint16_t bcnt = 0xa10;	// -1520 & 0xfff

	for (int i = 0; i < 8; i++) {
		this->rx_descr[i].rx_buffer = (((uint32_t)rx_buffer & 0x00ffffff) + (1520 * i));
		this->tx_descr[i].tx_buffer = (((uint32_t)tx_buffer & 0x00ffffff) + (1520 * i));

		this->rx_descr[i].bcnt = bcnt;
		this->rx_descr[i].ones = 0xf;
		this->tx_descr[i].bcnt = bcnt;
		this->tx_descr[i].ones = 0xf;

		this->rx_descr[i].own = 1;
	}

	// Подготавливаем блок инициализации
	this->init_block.rlen = 3;
	this->init_block.tlen = 3;

	this->init_block.rdra = ((uint32_t)this->rx_descr & 0x00ffffff);
	this->init_block.tdra = ((uint32_t)this->tx_descr & 0x00ffffff);

	memcpy(this->init_block.mac, mac, 6);

	this->write_csr32(1, (uint32_t)&this->init_block & 0xffff);
	this->write_csr32(2, ((uint32_t)&this->init_block >> 16) & 0x00ff);

	uint32_t csr0 = this->read_csr32(0);
	csr0 |= 1 | (1 << 6);
	this->write_csr32(0, csr0);

	uint32_t csr3 = this->read_csr32(3);
	csr3 &= -0x700;
	this->write_csr32(3, 0);

	while(!init_done);

	/*csr0 = this->read_csr32(0);
	csr0 |= (1 << 1) | (1 << 6);
	this->write_csr32(0, csr0);*/


	csr0 = this->read_csr32(0);
	csr0 &= -(0b101);
	csr0 |= 0b10;
	this->write_csr32(0, csr0);

}