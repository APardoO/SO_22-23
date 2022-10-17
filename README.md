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
