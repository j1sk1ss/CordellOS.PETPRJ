# CordellOS.MPRJ
Simple OS based on .asm / .c double stage boot loader, and .c / .cpp based kernel with GCC .c / .cpp compiler. 
</br>
//===================================================================</br>
// TODO List:</br>
//===================================================================</br>
//      1) Multiboot struct                                       [V]</br>
//      2) Phys and Virt manages                                  [V]</br>
//      3) ELF check v_addr                                       [V]</br>
//          3.1) Fix global and static vars                       [V]</br>
//          3.2) Loading ELF without malloc for fdata             [V]</br>
//      4) Paging (create error with current malloc) / allocators [V]</br>
//          4.0) Random Page fault (Null error de Italia)         [V]</br>
//          4.1) Tasking with paging                              [V]</br>
//          4.2) ELF exec with tasking and paging                 [V]</br>
//      5) VBE / VESA                                             [V]</br>
//          5.0) VBE kernel                                       [V]</br>
//      6) Keyboard to int                                        [V]</br>
//      7) Reboot outportb(0x64, 0xFE);                           [V]</br>
//      8) Mouse support                                          [V]</br>
//          8.0) Std lib for graphics                             [?]</br>
//          8.1) Loading BMP without malloc for fdata             [V]</br>
//          8.1) Syscalls to std libs                             [V]</br>
//          8.2) VBE userland                                     [ ]</br>
//      9) Malloc optimization                                    [?]</br>
//      10) Bags                                                  [?]</br>
//          10.0) Tasking page fault                              [?]</br>
//          10.1) Mouse page fault                                [V]</br>
//          10.2) Tasking with page allocator                     [ ]</br>
//      11) DOOM?                                                 [ ]</br>
//===================================================================</br>

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/cover.png)
