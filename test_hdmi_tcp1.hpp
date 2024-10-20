
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/vreg.h"
#include "pico/sem.h"

#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "sprite.h"




#define LED1_PIN            19
#define Led1Off()           gpio_put(LED1_PIN, 1);
#define Led1On()            gpio_put(LED1_PIN, 0);


void ProcessCommand(unsigned char *buf, unsigned int Size);

#include "w5500_config_tcp.h"

#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
#define DATA_BUF_SIZE		ETHERNET_BUF_MAX_SIZE
/* Socket */
#define SOCKET_LOOPBACK 0

/* Port */
#define PORT_LOOPBACK 5000

static uint8_t g_tcp_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};

int32_t Process_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t ret;
   uint16_t size = 0, sentsize=0;

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
			setSn_IR(sn,Sn_IR_CON);
         }
		 if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
			ret = recv(sn, buf, size);

			ProcessCommand(buf, ret);
         }
         break;

      case SOCK_CLOSE_WAIT :
         if((ret = disconnect(sn)) != SOCK_OK) return ret;

         break;

      case SOCK_INIT :
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;

      case SOCK_CLOSED:
         if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;

         break;
      default:
         break;
   }
   return 1;
}


// DVDD 1.2V (1.1V seems ok too)
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#define VREG_VSEL VREG_VOLTAGE_1_20
#define DVI_TIMING dvi_timing_640x480p_60hz

#define LED_PIN 16

struct dvi_inst dvi0;
#define FRAME_BUF_SIZE (FRAME_WIDTH * FRAME_HEIGHT)
uint16_t framebuf[FRAME_BUF_SIZE];

void core1_main() {
	dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
	dvi_start(&dvi0);
	dvi_scanbuf_main_16bpp(&dvi0);
	__builtin_unreachable();
}

void core1_scanline_callback() {
	// Discard any scanline pointers passed back
	uint16_t *bufptr;
	while (queue_try_remove_u32(&dvi0.q_colour_free, &bufptr))
		;
	// // Note first two scanlines are pushed before DVI start
	static uint scanline = 2;
	bufptr = &framebuf[FRAME_WIDTH * scanline];
	queue_add_blocking_u32(&dvi0.q_colour_valid, &bufptr);
	scanline = (scanline + 1) % FRAME_HEIGHT;
}



unsigned int gCmdStatus = 0;
unsigned int gImgIdx =0;
unsigned int gImgSize =0;
unsigned int gErrCnt = 0;
void ProcessCommand(unsigned char *buf, unsigned int Size)
{
	char *ptr;
	int size = 0;
	int i;
	unsigned int pixel_data = 0;

	if(gCmdStatus == 1)
	{
		for(i=0;i<Size/2;i++)
		{
			pixel_data = (buf[2*i]) | buf[2*i+1]<<8;

			if(gImgIdx>FRAME_BUF_SIZE)gImgIdx = FRAME_BUF_SIZE-1;
			framebuf[gImgIdx++] = pixel_data;
		}

		if(gImgIdx>=gImgSize)
		{
			printf("%d,%d, %d\r\n", Size, gImgIdx, gErrCnt);

			gCmdStatus = 0;
			gImgIdx = 0;
			gErrCnt =0 ;
		}
		gErrCnt++;

		if(gErrCnt>1000)
		{
			gCmdStatus = 0;
			printf("Timout %d,%d\r\n", Size, gImgIdx);
		}
	}
	else
	{
		if(strstr((const char*)buf, "CMD_LED_ON"))
		{
			Led1On();
			printf("Led On Command\r\n");
		}
		else if(strstr((const char*)buf, "CMD_LED_OFF"))
		{
			Led1Off();
			printf("Led Off Command\r\n");
		}
		else 
		{
			ptr = strstr((const char*)buf, "CMD_BMP_SEND");

			if(ptr)
			{
				gCmdStatus = 1;
				gImgIdx = 0;
				gErrCnt = 0;

         		//printf(ptr+strlen("CMD_BMP_SEND"));
				gImgSize = atoi((const char*)(ptr+strlen("CMD_BMP_SEND")));
				if(gImgSize>FRAME_BUF_SIZE)gImgSize = FRAME_BUF_SIZE-1;
				printf("bmp size = %d.\r\n", gImgSize);
			}
		}		
	}
}

int main() {
	int cnt  = 0;
	int y = 0;
	int x = 0;
    stdio_init_all();
	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
#ifdef RUN_FROM_CRYSTAL
	set_sys_clock_khz(12000, true);
#else
	// Run system at TMDS bit clock
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);
#endif

	gpio_init(LED1_PIN);
	gpio_set_dir(LED1_PIN, GPIO_OUT);

	printf("Configuring DVI..\n");
	InitEthernet();

	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi0.scanline_callback = core1_scanline_callback;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

	// Once we've given core 1 the framebuffer, it will just keep on displaying
	// it without any intervention from core 0
	sprite_fill16(framebuf, 0xffff, FRAME_WIDTH * FRAME_HEIGHT);
	uint16_t *bufptr = framebuf;
	queue_add_blocking_u32(&dvi0.q_colour_valid, &bufptr);
	bufptr += FRAME_WIDTH;
	queue_add_blocking_u32(&dvi0.q_colour_valid, &bufptr);

	printf("Core 1 start\n");
	multicore_launch_core1(core1_main);

	printf("Start rendering\n");

	while (1)
	{
		__wfe();
		Process_tcps(0, g_tcp_buf, 5000);
	}
	__builtin_unreachable();
}

