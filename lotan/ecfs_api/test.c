#include <stdio.h>
#include "libecfs.h"

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("%s file\n", argv[0]);
		exit(0);
	}
	int i, ret;
	ecfs_elf_t *desc;
	struct fdinfo *fdinfo;
	struct elf_prstatus *prstatus;
	ecfs_sym_t *dsyms, *lsyms;
	char *path;
	siginfo_t siginfo;

	desc = load_ecfs_file(argv[1]);
	path = get_exe_path(desc);
	
	printf("executable: %s\n", path);
	ret = get_thread_count(desc);
	
	printf("# of threads: %d\n", ret);
	
	ret = get_siginfo(desc, &siginfo);
	printf("Exited on signal %d\n", siginfo.si_signo);
	ret = get_prstatus_structs(desc, &prstatus);
	
	for (i = 0; i < ret; i++) 
		printf("(thread %d) pid: %d\n", i + 1, prstatus[i].pr_pid);

	ret = get_fd_info(desc, &fdinfo);
	
	for (i = 0; i < ret; i++) {
	 	printf("fd: %d path: %s\n", fdinfo[i].fd, fdinfo[i].path);
		if (fdinfo[i].net) {
			printf("src addr: %s:%d\n", inet_ntoa(fdinfo[i].socket.src_addr), fdinfo[i].socket.src_port);
			printf("dst addr: %s:%d\n", inet_ntoa(fdinfo[i].socket.dst_addr), fdinfo[i].socket.dst_port);
		}
		printf("\n");
	}
	ret = get_dynamic_symbols(desc, &dsyms);
	for (i = 0; i < ret; i++)
		printf("dynamic symbol: %s\n", &desc->dynstr[dsyms[i].nameoffset]);
	
	ret = get_local_symbols(desc, &lsyms);
	for (i = 0; i < ret; i++)
		printf("local symbol: %s\n", &desc->strtab[lsyms[i].nameoffset]);
	
	
	uint8_t *ptr;
	ssize_t len = get_ptr_for_va(desc, desc->textVaddr, &ptr);
	printf("%d bytes left for segment\n", (int)len);
	for (i = 0; i < 16; i++)
		printf("%02x ", ptr[i] & 0xff);
	printf("\n");
}	

