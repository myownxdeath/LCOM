#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "video_gr.h"
#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xE0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */



void *vg_init(unsigned short mode) {
	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1 << 14 | 0x105; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != 0) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;

	}

	int ans;
	struct mem_range mr;

	unsigned int vram_base = VRAM_PHYS_ADDR;/*VRAM’s physical addresss*/
	unsigned int vram_size = V_RES * H_RES * BITS_PER_PIXEL / 8; /*VRAM’s size, but you can use the frame-buffer size, instead*/
	//void* video_mem_ptr; /*frame-buffer VM address*/
	/*Allow memory mapping*/
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (ans = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", ans);
	h_res = H_RES;
	/*Map memory*/

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED)
		panic("couldn’t map video memory");

	return video_mem;
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void paint_pixel(unsigned short x, unsigned short y, unsigned long color) {
	char * add_it = video_mem;
	add_it += x + h_res * y;
	*add_it = color;

}