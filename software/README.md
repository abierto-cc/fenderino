# Fenderino Shield - code and libraries

Esta carpeta contiene los ejemplos de código que necesitas para poder empezar a experimentar con tu Fenderino. La subcarpeta *examples* tiene todo lo que necesitas para poder empezar a experimentar

En cualquier caso, para poder probar la carpeta *examples* hay un momento en el que necesitas librerias. Hemos incluido bajo *libraries* dos librerías que debieras instalar dentro de tu carpeta de sketchbook del IDE de Arduino. Es posible que ya tengas estas librerías, no hemos hecho ninguna modificación sobre ellas, sólo las compartimos para que sea más sencillo el encontrarlas

# librería Synth

Esta librería la creó DZL en Illutron (Copenague) en el año 2014 y la puedes econtrar en https://github.com/dzlonline/the_synth . Se trata de una librería que genera hasta 4 tonos diferentes usando los tipos de ondas: ruido, cuadradra, triangular, sinusoide. Estos cuatro tonos simultáneos se pueden asignar a un solo pin con lo que puedes crear tus propios instrumentos al crear el timbre del instrumento usando la suma de los tonos 

El código original de DZL no presenta una licencia de uso, por lo que nosotros le asignamos GPLv3 hasta que nos digan lo contrario

# librería Talkie

Esta librería la creó Peter Knight en el año 2011 y la puedes econtrar en https://github.com/going-digital/Talkie . Se trata de una librería de sńtesis de voz que puede reproducir sonidos almacenados como una tabla de ondas comprimida usando tablas de ruido y envolventes. Es una técnica desarrollada en el pasado entre otros por Texas Instruments para la creación de sistemas de voz con microcontroladores teniendo el sonido grabado en memoria de programa

La usamos para el ejemplo en el que Fenderino dirá los números en inglés al presionar los botones. Además, la carpeta de la librería incluye código de Freemat para comprimir otros ficheros de audio

Aquí un reto para quien se anime: crear un algoritmo en Java que sea capaz de tomar un fichero WAV y generar un array de audio comprimido en formato texto para poder incluir en los programas hechos ocn Arduino. Abierto se compromete a hacer una herramienta para el IDE de Arduino que use ese código para automáticamente añadir el sonido a tus programas

Esta librería está licenciada bajo GPLv2, por lo que la licencia GPLv3 usada para todo el documento resulta aplicable
