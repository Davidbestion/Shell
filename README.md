# Shell
Proyecto de Sistema Operativo. 
Integrante: David Sanchez Iglesias.
Este proyecto de Shell contiene las implementaciones basicas orientadas en el proyecto y, aunque no tuve tiempo de probarlo, teoricamente deberian funcionar unas pocas de las opcionales.
A grandes rasgos, mi proyecto extrae lo escrito en consola y es dividido en fragmentos delimitados por espacios. Dichos fragmentos son almacenados en tokens y con estos se construye un Abstract Syntax Tree (AST). 
El AST es una estructura bastante simple de entender. Un arbol representa un comando con sus respectivos argumentos. Su primer hijo contiene al token del comando como tal y el resto contiene, cada uno a un argumento. Cada hijo guarda referencias a su siguiente hermano y al anterior, asi se facilita el desplazamiento a traves de hermanos.
Ahora en cuanto a la estructura:
el archivo source.c contiene los metodos para leer de la consola. Sus funciones permiten tomar o soltar cada caracter a medida que se lee de la consola y otro para eliminar los espacios en blanco.
el archivo scanner.c crea los tokens a partir de lo que se extrajo de la consola haciendo uso de las funciones del archivo anterior.
el node.c contiene los metodos para crear el AST y moverse a traves de el.
el parser.c es el intermediario entre el scanner.c y el node.c, y es el que arma los AST como tal, usando las funciones de uno y otro archivo para construir los AST y darles valor a los nodos.
el executor.c, como su nombre indica, ejecuta los comandos. Sus metodos reciben un string con el nombre de un comando o un AST.
el prompt.c simplemente imprime el prompt de la consola.
El executor es capaz de procesar cualquier comando cuyo archivo este en el PATH de la computadora, el "cd" y, teoricamente, el "|" y los de redireccion ">", ">>" y "<".
En cuanto al comando "|", el programa me devolvia "segmentation fault" cada vez que lo probaba sin embargo nunca entendi por que me da ese error. El error ocurre dentro del metodo find_pipe_command, dentro del segundo while, dentro del if. Deberia ser en la linea 239 si se abre con el vs code. La idea de este metodo es buscar cual de los hijos en el AST posee el "|". Una vez encontrado, separo el AST en dos, uno con todos los hijos hasta el del "|" y otro con el resto de manera que ninguno contenga al del "|". Luego creo dos procesos, en cada uno se ejecuta un comando, o sea, se procesa uno de los dos AST nuevos, como si fueran nuevos comandos. El proceso padre espera a que finalice el hijo para entonces recibir lo que este halla devuelto.
Cabe aclarar que en el find_pipe_command se comienza por el ultimo hijo del AST con la intencion de incluir la funcion multipipe. Se analiza el AST de atras hacia adelante para garantizar que el ultimo comando solo se ejecute cuando todos los demas anteriores se hayan ejecutado. Este metodo encuentra el ultimo operador "|", divide el AST en 2 y los procesa de derecha a izquierda, haciendo que el derecho espere al izquierdo. Si al procesar el izquierdo, se encuentra otro operador "|", se repite el mecanismo. Asi se garantiza, teoricamente, que el ultimo de izquierda a derecha reciba el resultado que paso por todos los demas procesos a los que se le hizo "|", cumpliendo asi con el multipipe.
En cuanto al "cd" cabe destacar que si se le pasa una direccion donde uno de los archivos contiene un nombre con espacio, por ejemplo una carpeta con nombre "Tarea evaluativa", el programa dara error. Fuera de este detalle, el operador funciona perfectamente.
En cuanto a los espacios, no importa cuantos espacios haya entre dos comandos, los procesa como si fueran uno solo.
Espero que este proyecto le sea util y que lo disfrute.
