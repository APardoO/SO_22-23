# SO_22-23

>Authors: [@ApardoO](https://www.github.com/ApardoO) [@Hugoocb](https://www.github.com/Hugoocb)

Sistemas Operativos_Facultad de Ingeniería Informática_curso 2022-2023

En este repositorio se verán las prácticas de la asignatura de Sistemas Operativos, impartida en el tercer cuatrimestre del Grado en Ingeniería Informática. Se recogerán tanto los archivos pdf de la asignatura, los cuales contienen tanto los pasos a seguir como la especificación de los comandos a implementar por práctica, como los archivos con el código fuente del programa.

## Dependences

En caso de un mal funcionamiento, ejecutar los siguientes comandos para resolver las posibles dependencias:

```bash
sudo apt-get update
sudo apt-get install make valgrind -y
```

## Running Tests

Para comprobar que la práctica se compila, ejecuta y libera la memoria reservada, podemos usar el comando `make` dentro de los directorios de prácticas con los archivos `makefile` de configuración adecuado para cada práctica y valgrind para la comprobación de memporia. Para hacerlo rápido ejecutar el siguiente comando cambiando el nombre `output_file` por el correspondiente:

```bash
make && valgrind --tool=memcheck ./bin/output_file
```

## Practica 0

En esta práctica, se tomará contacto con el lenguaje C. Crearemos una shell interactiva, se añadirán comandos en las próximas prácticas o versiones de la shell. Nuestra shell se basará en cinco fases:

* Mostrar el _prompt_
* Lectura mediante la librería estándar de entrada y salida, el imput de un comando
* Guardar, si es válido el comando introducido en una lista histórica de comandos
* Separar el comando en argumentos
* Procesar el comadno con sus argumentos

### Comandos

| Command   | Paramether | Description                                                         |
| :-------- | :--------- | :------------------------------------------------------------------ |
| `autores` |            | Devuelve el nombre y los logins de los autores                      |
|           | `-l`       | Devuelve solo los loguins de los autores                            |
|           | `-n`       | Devuelve solo los nombres de los autores                            |
| `pid`     |            | Muestra el PID del proceso que ejecuta la shell                     |
|           | `-p`       | Muestra el PID del proceso de la shell                              |
| `carpeta` |            | Muestra el directorio actual                                        |
|           | `path`     | Cambia el directorio de trabajo al especificado                     |
| `fecha`   |            | Muestra tanto la fecha como la hora                                 |
|           | `-d`       | Muestra la fecha en formato `DD/MM/YYY`                             |
|           | `-h`       | Muestra la hora en formato `hh:mm:ss`                               |
| `hist`    |            | Muestra toda la lista del historico                                 |
|           | `-c`       | Limpia la lista del histórico                                       |
|           | `-N`       | Muestra los primeros `N` elementos                                  |
| `comando` | `N`        | Repite el comando `N` del histórico                                 |
| `infosis` |            | Muestra por pantalla información de la máquina que ejecuta la shell |
| `auyda`   |            | Muestra por pantalla la ayuda de todos los comadnos                 |
|           | `cmd`      | Muestra por pantalla la ayuda asociada al comando especificado      |
| `fin`     |            | Sale de la shell                                                    |
| `salir`   |            | Sale de la shell                                                    |
| `bye`     |            | Sale de la shell                                                    |

### Clues

Descripción básica de la shell:

```c
while(!terminado){
  imprimirPrompt();     # Llamada a printf()
  leerEntrada();        # Llamada a fgets()
  procesarEntrada();
}
```

El primer paso es `procesar la entrada` separando en parametros los argumentos del comando introducido, para ello se faculita la función `TrocearCadena()`:

```c
int TrocearCadena(char * cadena, char * trozos[]){
  int i=1;
  if ((trozos[0]=strtok(cadena, " \n\t"))==NULL)
    return 0;
  while ((trozos[i]=strtok(NULL, " \n\t"))!=NULL)
    i++;
  return i;
}
```

### Códigos de C en Linux

```c
#define EPERM           1   /* Operation not permitted */
#define ENOENT          2   /* No such file or directory */
#define ESRCH           3   /* No such process */
#define EINTR           4   /* Interrupted system call */
#define EIO             5   /* I/O error */
#define ENXIO           6   /* No such device or address */
#define E2BIG           7   /* Argument list too long */
#define ENOEXEC         8   /* Exec format error */
#define EBADF           9   /* Bad file number */
#define ECHILD          10  /* No child processes */
#define EAGAIN          11  /* Try again */
#define ENOMEM          12  /* Out of memory */
#define EACCES          13  /* Permission denied */
#define EFAULT          14  /* Bad address */
#define ENOTBLK         15  /* Block device required */
#define EBUSY           16  /* Device or resource busy */
#define EEXIST          17  /* File exists */
#define EXDEV           18  /* Cross-device link */
#define ENODEV          19  /* No such device */
#define ENOTDIR         20  /* Not a directory */
#define EISDIR          21  /* Is a directory */
#define EINVAL          22  /* Invalid argument */
#define ENFILE          23  /* File table overflow */
#define EMFILE          24  /* Too many open files */
#define ENOTTY          25  /* Not a typewriter */
#define ETXTBSY         26  /* Text file busy */
#define EFBIG           27  /* File too large */
#define ENOSPC          28  /* No space left on device */
#define ESPIPE          29  /* Illegal seek */
#define EROFS           30  /* Read-only file system */
#define EMLINK          31  /* Too many links */
#define EPIPE           32  /* Broken pipe */
#define EDOM            33  /* Math argument out of domain of func */
#define ERANGE          34  /* Math result not representable */

#define EDEADLK         35  /* Resource deadlock would occur */
#define ENAMETOOLONG    36  /* File name too long */
#define ENOLCK          37  /* No record locks available */
#define ENOSYS          38  /* Function not implemented */
#define ENOTEMPTY       39  /* Directory not empty */
#define ELOOP           40  /* Too many symbolic links encountered */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */
#define ENOMSG          42  /* No message of desired type */
#define EIDRM           43  /* Identifier removed */
#define ECHRNG          44  /* Channel number out of range */
#define EL2NSYNC        45  /* Level 2 not synchronized */
#define EL3HLT          46  /* Level 3 halted */
#define EL3RST          47  /* Level 3 reset */
#define ELNRNG          48  /* Link number out of range */
#define EUNATCH         49  /* Protocol driver not attached */
#define ENOCSI          50  /* No CSI structure available */
#define EL2HLT          51  /* Level 2 halted */
#define EBADE           52  /* Invalid exchange */
#define EBADR           53  /* Invalid request descriptor */
#define EXFULL          54  /* Exchange full */
#define ENOANO          55  /* No anode */
#define EBADRQC         56  /* Invalid request code */
#define EBADSLT         57  /* Invalid slot */

#define EDEADLOCK       EDEADLK
  
#define EBFONT          59  /* Bad font file format */
#define ENOSTR          60  /* Device not a stream */
#define ENODATA         61  /* No data available */
#define ETIME           62  /* Timer expired */
#define ENOSR           63  /* Out of streams resources */
#define ENONET          64  /* Machine is not on the network */
#define ENOPKG          65  /* Package not installed */
#define EREMOTE         66  /* Object is remote */
#define ENOLINK         67  /* Link has been severed */
#define EADV            68  /* Advertise error */
#define ESRMNT          69  /* Srmount error */
#define ECOMM           70  /* Communication error on send */
#define EPROTO          71  /* Protocol error */
#define EMULTIHOP       72  /* Multihop attempted */
#define EDOTDOT         73  /* RFS specific error */
#define EBADMSG         74  /* Not a data message */
#define EOVERFLOW       75  /* Value too large for defined data type */
#define ENOTUNIQ        76  /* Name not unique on network */
#define EBADFD          77  /* File descriptor in bad state */
#define EREMCHG         78  /* Remote address changed */
#define ELIBACC         79  /* Can not access a needed shared library */
#define ELIBBAD         80  /* Accessing a corrupted shared library */
#define ELIBSCN         81  /* .lib section in a.out corrupted */
#define ELIBMAX         82  /* Attempting to link in too many shared libraries */
#define ELIBEXEC        83  /* Cannot exec a shared library directly */
#define EILSEQ          84  /* Illegal byte sequence */
#define ERESTART        85  /* Interrupted system call should be restarted */
#define ESTRPIPE        86  /* Streams pipe error */
#define EUSERS          87  /* Too many users */
#define ENOTSOCK        88  /* Socket operation on non-socket */
#define EDESTADDRREQ    89  /* Destination address required */
#define EMSGSIZE        90  /* Message too long */
#define EPROTOTYPE      91  /* Protocol wrong type for socket */
#define ENOPROTOOPT     92  /* Protocol not available */
#define EPROTONOSUPPORT 93  /* Protocol not supported */
#define ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define EOPNOTSUPP      95  /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT    96  /* Protocol family not supported */
#define EAFNOSUPPORT    97  /* Address family not supported by protocol */
#define EADDRINUSE      98  /* Address already in use */
#define EADDRNOTAVAIL   99  /* Cannot assign requested address */
#define ENETDOWN        100 /* Network is down */
#define ENETUNREACH     101 /* Network is unreachable */
#define ENETRESET       102 /* Network dropped connection because of reset */
#define ECONNABORTED    103 /* Software caused connection abort */
#define ECONNRESET      104 /* Connection reset by peer */
#define ENOBUFS         105 /* No buffer space available */
#define EISCONN         106 /* Transport endpoint is already connected */
#define ENOTCONN        107 /* Transport endpoint is not connected */
#define ESHUTDOWN       108 /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109 /* Too many references: cannot splice */
#define ETIMEDOUT       110 /* Connection timed out */
#define ECONNREFUSED    111 /* Connection refused */
#define EHOSTDOWN       112 /* Host is down */
#define EHOSTUNREACH    113 /* No route to host */
#define EALREADY        114 /* Operation already in progress */
#define EINPROGRESS     115 /* Operation now in progress */
#define ESTALE          116 /* Stale NFS file handle */
#define EUCLEAN         117 /* Structure needs cleaning */
#define ENOTNAM         118 /* Not a XENIX named type file */
#define ENAVAIL         119 /* No XENIX semaphores available */
#define EISNAM          120 /* Is a named type file */
#define EREMOTEIO       121 /* Remote I/O error */
#define EDQUOT          122 /* Quota exceeded */

#define ENOMEDIUM       123 /* No medium found */
#define EMEDIUMTYPE     124 /* Wrong medium type */
#define ECANCELED       125 /* Operation Canceled */
#define ENOKEY          126 /* Required key not available */
#define EKEYEXPIRED     127 /* Key has expired */
#define EKEYREVOKED     128 /* Key has been revoked */
#define EKEYREJECTED    129 /* Key was rejected by service */

/* for robust mutexes */
#define EOWNERDEAD      130 /* Owner died */
#define ENOTRECOVERABLE 131 /* State not recoverable */

```

## Practica 1

Cuando una salida del programa ejecutado no es satisfacctoria, se devuelve un código esoecial (generalmente -1) que se almacenará en una variable global del sistema `errno` para códigos de error. `man` proporciona información de por qué se devuelve dicho código de error.

* **perror()**: Muestra por la salida estándar un mensaje de error (por pantalla, si no se ha redireccionado la salida).
* **strerror()**: Retorna un puntero con la cadena de texto con el mensaje de error relacionado a un código pasado por parámetro.
* **extern char * sys_errlist[]**: Matriz que contiene la descripción de error indexada por número de error para que `sys_errlist[errno]` tenga una descripción del error asociado con errno.

### Comandos

| Command   | Paramether            | Description                                                 |
| :-------- | :-------------------- | :---------------------------------------------------------- |
| `create`  | `name`                | Nombre o path del directorio a crear -> `drwxr-xr-x`        |
|           | `-f`                  | Crea un fichero                      -> `.rw-r--r--`        |
| `stat`    | `name1` `name2` `...` | Muestra información de los archvos o directorios            |
|           | `-long [-acc] [-link]`| Formato largo {tiempo de acceso} {muestra el path del link} |
| `list`    | `name1` `name2` `...` | Lista el contenido de los directorios                       |
|           | `-hid`                | Muestra archivos/directorios ocultos                        |
|           | `-reca`               | Recursivo (antes)                                           |
|           | `-recb`               | Recursivo (despues)                                         |
|           | `*stat`               | Mismos parámetros que el comando stat                       |
| `delete`  | `name1` `name2` `...` | Borra ficheros o directorios vacíos                         |
| `deltree` | `name1` `name2` `...` | Borra fichers o directorios no vacíos recursivamente        |

### Clues

Para poder desenvolver la práctica podemos tomar una idea de los siguientes códigos para la práctica, tomadas de prácticas anteriores:

Para capturar los el tipo de archivo:

```c
char LetraTF (mode_t m){
	switch (m&S_IFMT){	/*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK:	return ’s’; /*socket */
		case S_IFLNK:	return ’l’;	/*symbolic link*/
		case S_IFREG:	return ’-’;	/* fichero normal*/
		case S_IFBLK:	return ’b’;	/*block device*/
		case S_IFDIR:	return ’d’;	/*directorio */
		case S_IFCHR:	return ’c’;	/*char device*/
		case S_IFIFO:	return ’p’;	/*pipe*/
		default: return ’?’;	/*desconocido, no deberia aparecer*/
	}
}
```

Obtención de permisos por el primer método:

```c
char * ConvierteModo (mode_t m, char *permisos){
	strcpy (permisos,"---------- ");
	permisos[0]=LetraTF(m);
	if (m&S_IRUSR) permisos[1]=’r’;	/*propietario*/
	if (m&S_IWUSR) permisos[2]=’w’;
	if (m&S_IXUSR) permisos[3]=’x’;
	if (m&S_IRGRP) permisos[4]=’r’;	/*grupo*/
	if (m&S_IWGRP) permisos[5]=’w’;
	if (m&S_IXGRP) permisos[6]=’x’;
	if (m&S_IROTH) permisos[7]=’r’;	/*resto*/
	if (m&S_IWOTH) permisos[8]=’w’;
	if (m&S_IXOTH) permisos[9]=’x’;
	if (m&S_ISUID) permisos[3]=’s’;	/*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]=’s’;
	if (m&S_ISVTX) permisos[9]=’t’;
	return permisos;
}
```

Obtención de permisos por el segundo método:

```c
char * ConvierteModo2 (mode_t m){
	static char permisos[12];
	strcpy (permisos,"---------- ");

	permisos[0]=LetraTF(m);
	if (m&S_IRUSR) permisos[1]=’r’;	/*propietario*/
	if (m&S_IWUSR) permisos[2]=’w’;
	if (m&S_IXUSR) permisos[3]=’x’;
	if (m&S_IRGRP) permisos[4]=’r’;	/*grupo*/
	if (m&S_IWGRP) permisos[5]=’w’;
	if (m&S_IXGRP) permisos[6]=’x’;
	if (m&S_IROTH) permisos[7]=’r’;	/*resto*/
	if (m&S_IWOTH) permisos[8]=’w’;
	if (m&S_IXOTH) permisos[9]=’x’;
	if (m&S_ISUID) permisos[3]=’s’;	/*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]=’s’;
	if (m&S_ISVTX) permisos[9]=’t’;
	return (permisos);
}
```

Obtención de permisos por el tercer método:

```c
char * ConvierteModo3 (mode_t m){
	char * permisos;
	permisos=(char *) malloc (12);
	strcpy (permisos,"---------- ");

	permisos[0]=LetraTF(m);
	if (m&S_IRUSR) permisos[1]=’r’; /*propietario*/
	if (m&S_IWUSR) permisos[2]=’w’;
	if (m&S_IXUSR) permisos[3]=’x’;
	if (m&S_IRGRP) permisos[4]=’r’; /*grupo*/
	if (m&S_IWGRP) permisos[5]=’w’;
	if (m&S_IXGRP) permisos[6]=’x’;
	if (m&S_IROTH) permisos[7]=’r’; /*resto*/
	if (m&S_IWOTH) permisos[8]=’w’;
	if (m&S_IXOTH) permisos[9]=’x’;
	if (m&S_ISUID) permisos[3]=’s’; /*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]=’s’;
	if (m&S_ISVTX) permisos[9]=’t’;
	return (permisos);
}

```

Estructura de datos del tipo stat(`struct stat`), necesaria para comprender la práctica:

```c
struct stat {
    dev_t     st_dev;     // ID of device containing file
    ino_t     st_ino;     // inode number
    mode_t    st_mode;    // protection
    nlink_t   st_nlink;   // number of hard links
    uid_t     st_uid;     // user ID of owner
    gid_t     st_gid;     // group ID of owner
    dev_t     st_rdev;    // device ID (if special file)
    off_t     st_size;    // total size, in bytes
    blksize_t st_blksize; // blocksize for filesystem I/O
    blkcnt_t  st_blocks;  // number of blocks allocated
    time_t    st_atime;   // time of last access
    time_t    st_mtime;   // time of last modification
    time_t    st_ctime;   // time of last status change
};
```
