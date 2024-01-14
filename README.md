# CordellOS.MPRJ
This repository contains my attempts to build my own computer from scratch. This ReadME will contain all the information about this project, and this is where we will begin.

## Main idea 
The main idea, as you might guess, is to create my own computer (case, motherboard and OS), primarily in order to gain experience in low-level programming, engineering design, and, as an option, in design.

*Well, what to hide, I also wanted to come to the university with this work of art and try to pass at least some work on it...*

## Start of proccess
It all started with the idea of creating my own operating system to practice C and ASM skills. As a basis for my study, I chose a tutorial from nanobyte (links to all sources will be presented below) and the OsDev website.

Then, reaching the current end of the tutorial (currently 11/3/2023, video still in production), I completed the basic file system functions, integrated a simple ASM compiler with a simple shell language, and began selecting the case and board.

<p align="center">
  <img width="800" height="480" src="https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/6_cover.jpeg">
  <br>
    <text> *simple file manager* </text>
  </br>
</p>



<p align="center">
  <img width="800" height="480" src="https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/7_cover.jpeg">
    <br>
      <text> *Start screen with login process* </text>
    </br>
</p>


## Case
I decided to print the body on a 3D printer. Well, for this it was necessary to first create a model for printing.
As a reference, or basis, I chose the cases of old commodore and atari computers. Well, something like this:

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/1_cover.jpg)

*just old pc in keyboard. Don`t know what is this model*

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/2_cover.png)

*ATARI pc*

After modeling the model in **Blender**, I ended up with approximately the following design (Of course, I prepared for printing and left all the necessary connectors for the components):

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/3_cover.png)

And that`s how looks model in special app for 3D printing:
![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/5_cover.jpg)

## Motherboard
The motherboard was originally planned as a board based on an 8086 processor, initially assembled from components, but after estimating the final dimensions, and the need for soldering and purchasing a huge number of components (This would have turned out to be a rather expensive undertaking), I decided to turn my attention to the Rasberry Pi 3B.

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/covers/rasberryPi.jpg)

**TODO:**

- Syscalls
- ELF executing
- С or CPP in-build compiler
- Prepare for non-emulator device
- Print case
- Assemble

![Alt Text](https://github.com/j1sk1ss/CordellOS.MPRJ/blob/main/cover.png)
