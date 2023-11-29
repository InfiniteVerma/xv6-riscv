#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// all cores will have access to this variable
// volatile -> used for synchronization by multiple cores
volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  /*
   * Each core will begin this in parallel
   *
   * On core 0, we will execute below if statement
   *  - initializing things
   */
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting - core 0 init ongoing\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process

    /*
     * Telling the compiler to not optimize
     */
    __sync_synchronize();
    started = 1; // this variable is now set.
  } else {
    // rest of cores execute below code
    // when started is set (init has happened), then below will start executing 
    while(started == 0)
      ;

    /*
     * Telling the compiler to not optimize
     */
    __sync_synchronize();

    printf("hart %d starting\n", cpuid());

    /*
     * Per core initialization things. 
     */
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  /*
   * Each core calls this scheduler func
   * This starts looking for processes to execute
   */
  scheduler();        
}
