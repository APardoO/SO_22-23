There are three different folders for three different paltaforms: linux, FreeBSD and Mac-OS

Static executables do not need any library on the system.

Dynamically linked executables need libraries to be present (you can check which libraries with ldd). To execute the 32bit dynamically linked executable
on a 64 bit machine you'll need a 32 bit version of the libs (in some linux distros, package "gcc-multilib")

You'll probably use only one of the executables for your platform, although you can try the others (memory address space will differ).

Remmeber to give the shell execution permissions (chmod 755 your_shell_file) and as you probably do not have "." in your path execute with ./

Example (chmod to be done only once)

$ chmod 755 shell
$ ./shell

You can use commads "ayuda" or "help" inside the shell to get help

File ayudaP1.c at soures directory has some code you can use if you wish so


