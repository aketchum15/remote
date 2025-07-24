#include <alsa/asoundlib.h>
#include <sys/mman.h>
#include "inc/recorder.hxx"


#define PERIPHERAL_BASE  0xFE000000
#define GPIO_BASE  (PERIPHERAL_BASE + 0x200000)
#define CLK_BASE (PERIPHERAL_BASE + 0x101000)

#define BLK_SIZE 4096

#define GPCLK0_CTL 28
#define GPCLK0_DIV 29

volatile unsigned int *gpio;
volatile unsigned int *clk;


void init() {

    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("open /dev/mem");
        exit(1);
    }

    gpio = static_cast<unsigned int *>(mmap(NULL, BLK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE));
    clk  = static_cast<unsigned int *>(mmap(NULL, BLK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, CLK_BASE));

    if (gpio == MAP_FAILED || clk == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    close(mem_fd);

};

void set_gpio_alt_func(int gpio_pin, int alt) {

    int reg = gpio_pin / 10;          // Each FSEL register controls 10 GPIOs
    int shift = (gpio_pin % 10) * 3;  // Each pin takes 3 bits

    // Bit mask to wipe chunk of 3 and set it to alt 
    gpio[reg] = (gpio[reg] & ~(7 << shift)) | (alt << shift);
}

void output_19_2mhz_on_gpio4() {
    // Stop GPCLK0
    clk[GPCLK0_CTL] = 0x5A000000 | (1 << 5); // Disable GPCLK0
    usleep(10);

    // Use source 1 = oscillator (19.2 MHz), no divider
    clk[GPCLK0_DIV] = 0x5A000000 | (1 << 12);  // DIVI = 1
    usleep(10);

    // Enable clock, source = 1 (oscillator)
    clk[GPCLK0_CTL] = 0x5A000011;  // MASH=0, ENAB=1, SRC=oscillator
    usleep(10);
}

int main() {

    init();
    set_gpio_alt_func(4, 0);

    output_19_2mhz_on_gpio4();


    // Recorder r; 
    // r.setSoundDevice("hw:Device,0");

    // r.init();
    // r.record();
    return 0;
}
