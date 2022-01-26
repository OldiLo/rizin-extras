/* radare2-keystone - GPL - Copyright 2016 - pancake */

#include <rz_asm.h>
#include <rz_lib.h>
#include <keystone/keystone.h>
#include <keystone/ppc.h>
#include "keystone_priv.h"

static int assemble(RzAsm *a, RzAsmOp *ao, const char *str) {
	size_t i;
	bool mnem = true;
	char buffer[128];
	ks_mode mode = (ks_mode)0;
	switch (a->bits) {
	case 32:
		mode = (ks_mode)KS_MODE_PPC32;
		break;
	case 64:
		mode = (ks_mode)KS_MODE_PPC64;
		break;
	default:
		RZ_LOG_ERROR("invalid arch bits.\n");
		return -1;
	}
	mode = (ks_mode)((int)mode | KS_MODE_BIG_ENDIAN);

	// for some reasons keystone on ppc does not accept r0, r1, etc..
	// but accepts directly the register number: r0 = 0, r27 = 27, etc..
	// example: addis R7, r6, 0x0011 -> addis 7, 6, 0x0011
	snprintf(buffer, sizeof(buffer), "%s", str);
	for (i = 0; i < strlen(buffer); ++i) {
		if (mnem && IS_WHITESPACE(buffer[i])) {
			mnem = false;
		} else if (!mnem && (buffer[i] == 'R' || buffer[i] == 'r') && IS_WHITESPACE(buffer[i - 1])) {
			buffer[i] = ' ';
		}
	}

	return keystone_assemble(a, ao, buffer, KS_ARCH_PPC, mode);
}

#ifdef __cplusplus
extern "C" {
#endif

RzAsmPlugin rz_asm_plugin_ppc_ks = {
	.name = "ppc.ks",
	.arch = "ppc",
	.author = nullptr,
	.version = nullptr,
	.cpus = nullptr,
	.desc = "PowerPC keystone assembler",
	.license = "GPL",
	.bits = 32 | 64,
	.endian = RZ_SYS_ENDIAN_LITTLE | RZ_SYS_ENDIAN_BIG,
	.init = nullptr,
	.fini = nullptr,
	.disassemble = nullptr,
	.assemble = &assemble,
	.modify = nullptr,
	.mnemonics = nullptr,
	.features = nullptr,
	.platforms = nullptr,
};

#ifndef CORELIB
struct rz_lib_struct_t rizin_plugin = {
	.type = RZ_LIB_TYPE_ASM,
	.data = &rz_asm_plugin_ppc_ks,
	.version = RZ_VERSION,
	.free = nullptr,
	.pkgname = nullptr,
};
#endif

#ifdef __cplusplus
}
#endif
