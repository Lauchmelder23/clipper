#include "internal/havarie.h"

#include "internal/text_screen.h"

void capsize() {
	print_havarie_msg();

	asm("cli\n"
		"hlt");

	for(;;);
}

void print_havarie_msg() {
	tsclear_screen();
	tscursor_set(0, 0);
	tsputs("Havarie - the clipper has capsized\n\n\n", TEXT_SCREEN_BG_BLACK | TEXT_SCREEN_FG_LIGHT_CYAN);

	
	int eax, ebx, ecx, edx, esi, edi, esp, ebp;
	asm("mov %%eax, %[eax]\n"
		"mov %%ebx, %[ebx]\n"
		"mov %%ecx, %[ecx]\n"
		"mov %%edx, %[edx]\n"
		"mov %%esi, %[esi]\n"
		"mov %%edi, %[edi]\n"
		"mov %%esp, %[esp]\n"
		"mov %%ebp, %[ebp]\n"
		: [eax]"=m" (eax), [ebx]"=m" (ebx), [ecx]"=m" (ecx), [edx]"=m" (edx), 
		  [esi]"=m" (esi), [edi]"=m" (edi), [esp]"=m" (esp), [ebp]"=m" (ebp)
	);

	tsprintf("eax=0x%08x\t\tesi=0x%08x\n", eax, esi);
	tsprintf("ebx=0x%08x\t\tedi=0x%08x\n", ebx, edi);
	tsprintf("ecx=0x%08x\t\tesp=0x%08x\n", ecx, esp);
	tsprintf("edx=0x%08x\t\tebp=0x%08x\n", edx, ebp);
}
