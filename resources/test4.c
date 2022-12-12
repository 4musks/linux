#include <stdio.h>
#include <sys/types.h>

static inline void
__cpuid(unsigned int *eax, unsigned int *ebx, unsigned int *ecx,
unsigned int *edx)
{
asm volatile("cpuid"
: "=a" (*eax),
"=b" (*ebx),
"=c" (*ecx),
"=d" (*edx)
: "0" (*eax), "1" (*ebx), "2" (*ecx), "3" (*edx));
}

int
main(int argc, char **argv)
{
unsigned int eax, ebx, ecx, edx;
int i;

	for (i = 0; i < 70; ++i){
	eax = 0x4fffffff;
	ecx = i;
		printf("###################### All values are in HEX\n");
		printf("CPUID(0x4FFFFFFF) input,  eax=%x, ebx=%x, ecx=%x, edx=%x \n", eax, ebx, ecx, edx);
		__cpuid(&eax, &ebx, &ecx, &edx);
		printf("CPUID(0x4FFFFFFF) output, eax=%x, ebx=%x, ecx=%x, edx=%x \n", eax, ebx, ecx, edx);
	}
}
