#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	mmap_t mmap;
	lm_init();
	if (lm_alloc(sizeof(vbe_mode_info_t), &mmap) == NULL)
		return 1;
	struct reg86u r;

	r.u.w.ax = 0x4F01; /*VBE get mode info*/
	/*translate the buffer linear address to a far pointer*/
	r.u.w.es = PB2BASE(mmap.phys); /*set a segment base*/
	r.u.w.di = PB2OFF(mmap.phys); /*set the offset accordingly*/
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != OK) {/*call BIOS*/
		printf("set_vbe_mode: sys_int86() failed \n");
		lm_free(&mmap);
		return 1;
	}

	memcpy(vmi_p, mmap.virtual, mmap.size);
	lm_free(&mmap);
	return 0;
}

int vbe_get_controller_info(VbeInfoBlock *vib_p) {
	mmap_t mmap;
	lm_init();
	if (lm_alloc(sizeof(VbeInfoBlock), &mmap) == NULL)
		return 1;
	struct reg86u r;

	r.u.w.ax = 0x4F00; /*VBE get controller info*/
	/*translate the buffer linear address to a far pointer*/
	r.u.w.es = PB2BASE(mmap.phys); /*set a segment base*/
	r.u.w.di = PB2OFF(mmap.phys); /*set the offset accordingly*/
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != OK) {/*call BIOS*/
		printf("set_vbe_mode: sys_int86() failed \n");
		lm_free(&mmap);
		return 1;
	}

	memcpy(vib_p, mmap.virtual, mmap.size);
	lm_free(&mmap);
	return 0;
}
