#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include "threads/init.h"

void syscall_entry (void);
void syscall_handler (struct intr_frame *);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void
syscall_init (void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

/* The main system call interface */
void
syscall_handler (struct intr_frame *f ) {
	// TODO: Your implementation goes here.
	if(f->R.rax ==  SYS_HALT){
		//printf("sys_halt\n");
		power_off();
	}
	else if(f->R.rax ==  SYS_EXIT){ 
		//printf("sys_exit\n");
		thread_exit(f->R.rdi);	
	}
	else if(f->R.rax == SYS_FORK){ 
		//printf("sys_fork\n");
		f->R.rax = process_fork(f->R.rdi, f);
	}
	else if (f->R.rax == SYS_EXEC){
		//printf("sys_exec\n");
		char cmd[strlen(f->R.rdi) +1];
		strlcpy(cmd, f->R.rdi, sizeof(cmd));
		if(process_exec(cmd) == -1)
			thread_exit(-1);	
	}
	else if(f->R.rax == SYS_WAIT){ 
		//printf("sys_wait\n");
		f->R.rax = process_wait(f->R.rdi);
	}
	else if(f->R.rax == SYS_WRITE){
		//printf("sys_write\n");
		if(f->R.rdi == 1)
			putbuf((char *)f->R.rsi, f->R.rdx);
	}
	//printf ("system call!\n");
	do_iret(f);
}
