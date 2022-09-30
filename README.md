# SO_22-23
Asignatura de Sistemas Operativos de la Facultad de Informática, curso 2022-2023

En este repositorio se verán las prácticas de la asignatura de Sistemas Operativos
de la facultad de informática de la UDC. Se recogerán tanto los archivos pdf de la
asignatura, los cales contienen la especificacion de lo que se va a tratar, como
la documentacion de los comandos a tratar. Mas abajo se destacan los comandos tratados
para las distintas prácticas:

## Práctica 0:
======================
-autores		[-l|-n]
>[ ]\tDevuelve el nombre y los logins de los autores
>[l]\tDevuelve solo los logins de los autores
>[n]\tDevuelve solo los nombres de los autores

-pid			[-p]
>[ ]\tMuestra el PID del proceso que ejecuta la shell
>[p]\tMuestra el PID del proceso de la shell

-carpeta		[direct]
>[ ]\t\t\tMuestra el directorio actual
>[direct]\tCambia el directorio de trabajo al especificado

-fecha		[-d|-h]
>[ ]\tMuestra tanto la fecha como la hora
>[d]\tMuestra la fecha en formato DD/MM/YYYY
>[h]\tMuestra la hora en formato hh:mm:ss

-hist		[-c|-N]
>[ ]\tMuestra toda la lista del historico
>[c]\tLimpia la lista del historico
>[N]\tMuestra los primeros N elementos

-commando	N
>Repite el comando N del historico

-infosis
>Muestra por pantalla información de la máquina que ejecuta la shell

-ayuda		[cmd]
>[ ]\t\tMuestra por pantalla la ayuda de todos los comandos
>[cmd]\tMuestra por pantalla la ayuda asociada al comando especificado

-fin		Sale de la shell
-salir	Sale de la shell
-bye		Sale de la shell


## Práctica 1:
======================
create		[file]
	[ ]
	[f]		Crea un fichero
	[file]	Nombre del archivo o directorio a crear

stat		[name1] [name2] [...]
	[ ]		Muestra informacion de los archivos o directorios
	[long]	Listado largo
	[acc]	Muestra el tiempo de acceso
	[link]	Muestra el path contenido si es un enlace simbólico
	[name*]	Nombre de los archivos o directorios

list		[n1] [n2] [...]
	[ ]		Lista el contenido de directorios
	[hid]	Muestra archivos/directorios ocultos
	[reca]	Recursivo (antes)
	[recb]	Recursivo (despues)
	[$stat]	Mismos parámetros que el comando stat

delete		[name1] [name2] [...]
	[ ]		Borra ficheros o directorios vacíos

deltree		[name1] [name2] [...]
	[ ]		Borra ficheros o directorios no vacíos recursivamente
