# CordellOS.MPRJ
Simple OS based on .asm / .c double stage boot loader, and .c / .cpp based kernel with GCC .c / .cpp compiler. 
</br>
//===================================================================</br>
// TODO List:</br>
//===================================================================</br>
//      &nbsp;1) Multiboot struct                                       [V]</br>
//      &nbsp;2) Phys and Virt manages                                  [V]</br>
//      &nbsp;3) ELF check v_addr                                       [V]</br>
//          &nbsp;&nbsp;&nbsp;3.1) Fix global and static vars                       [V]</br>
//          &nbsp;&nbsp;&nbsp;3.2) Loading ELF without malloc for fdata             [V]</br>
//      &nbsp;4) Paging (create error with current malloc) / allocators [V]</br>
//          &nbsp;&nbsp;&nbsp;4.0) Random Page fault (Null error de Italia)         [V]</br>
//          &nbsp;&nbsp;&nbsp;4.1) Tasking with paging                              [V]</br>
//          &nbsp;&nbsp;&nbsp;4.2) ELF exec with tasking and paging                 [V]</br>
//      &nbsp;5) VBE / VESA                                             [V]</br>
//          &nbsp;&nbsp;&nbsp;5.0) VBE kernel                                       [V]</br>
//      &nbsp;6) Keyboard to int                                        [V]</br>
//      &nbsp;7) Reboot outportb(0x64, 0xFE);                           [V]</br>
//      &nbsp;8) Mouse support                                          [V]</br>
//          &nbsp;&nbsp;&nbsp;8.0) Std lib for graphics                             [?]</br>
//          &nbsp;&nbsp;&nbsp;8.1) Loading BMP without malloc for fdata             [V]</br>
//          &nbsp;&nbsp;&nbsp;8.1) Syscalls to std libs                             [V]</br>
//          &nbsp;&nbsp;&nbsp;8.2) VBE userland                                     [ ]</br>
//      &nbsp;9) Malloc optimization                                    [?]</br>
//      &nbsp;10) Bags                                                  [?]</br>
//          &nbsp;&nbsp;&nbsp;10.0) Tasking page fault                              [?]</br>
//          &nbsp;&nbsp;&nbsp;10.1) Mouse page fault                                [V]</br>
//          &nbsp;&nbsp;&nbsp;10.2) Tasking with page allocator                     [ ]</br>
//      &nbsp;11) DOOM?                                                 [ ]</br>
//===================================================================</br>

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/cover.png)
