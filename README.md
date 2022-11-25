# Laboratorios de Física básica
En este readme se detallan las partes mas importantes del proyecto de remotización del laboratorio de física básica, haciendo énfacis en el hardware.

## Protocolo para los laboratorios de "Física básica"
Para poder establecer una comunicación entre al api web, generada por el equipo de software del proyecto, y los dispositivos en el laboratorio establecemos un protocolo en común. 

El procedimiento de comunicación es el siguiente. Cada vez que se quiere modificar algun dato en el laboratorio. El Servidor debe enviar un:
```Ruby 
POST /HTTP/1.1 + <JSON a enviar>
```
#### Json a enviar (ejemplo)
```Ruby  
{ "Estado": [0,true,false],"Analogico": [1,125,542,2]}
```
Y para obtener avances de los datos o reporte de los datos debe enviar un:
```Ruby 
GET /HTTP/1.1
```
De esta forma el Arduino responde ante una petición.

#### Json a recibir (ejemplo)

```Ruby  
{ "Estado": [0,true,false],"Analogico": [1,125,542,2], "Error": "Mensaje de error"}
```

## Sintaxis

**Estado:** Es un array conformado por 3 elementos en el siguiente orden: [Laboratorio, SubLab, Inicio del experimento]

|Laboratorio  | Sub Laboratorio  | Inicio del experimento | Laboratorio Seleccionado | Estado del experimento|
|-|-----|-----|-----------------|--------|
|3|true |true |Lente Convergente|Inicia  |
|3|true |false|Lente Convergente|Finaliza|
|3|false|true |Lente Divergente |Inicia  |
|3|false|false|Lente Divergente |Finaliza|

**Elementos por Laboratorio**

***Convergente***

- Analogico: [Tipo_Diafragma,Distancia_FL,Distancia_LP]
    
    Distancia_FL: Corresponde a la distancia entre Foco y Lente.
    
    Distancia_LP: Corresponde a la distancia entre Lente y Pantalla.

- Tipo_Diafragma = Indica el tipo de diafragma
  - 0 : Sin diafragma
  - 1 : Central
  - 2 : Periferico
  - 3 : Filtro Rojo

***Divergente***

```Ruby 
Analogico:[Tipo_Diafragma, Distancia_FL1, Distancia_L2P, Distancia_L1L2]
```
- Distancia_FL1: Corresponde a la distancia entre Foco y Lente Divergente.
- Distancia_L2P: Corresponde a la distancia entre Lente convergente y Pantalla.
- Distancia_L1L2: Corresponde a la distancia entre Lente divergente y convergente.

## Valores permitidos

Los valores permitidos en las distancias son:

| Distancia          |Valor mín|Valor máx| Unidad |
| ------------------ |---------|---------|--------|
|FL: Foco Lente      |   120   |   970   |  [mm]  |
|LP: Lente Pantalla  |    70   |   970   |  [mm]  |
|FL1: Foco Lente1    |    50   |   920   |  [mm]  |
|L1L2: Lente1 Lente2 |    70   |   900   |  [mm]  |
|L2P: Lente2 Pantalla|    70   |   970   |  [mm]  |

Se puede observar en la imagen las distancias especificadas. Se consideran margenes entre los soportes por seguridad.

![Vista Superior Distancias](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_distancias.png)


**Error:** Es una variable numerica que representa un mensaje de error. 

| Tipo de error                     |  Valor  |
|-----------------------------------|---------|
| Sin errores                       |    0    |
| Error de distancia                |    1    |
| Error de tipo de diafragma        |    2    |
| Error de laboratorio incorrecto   |    3    |

## Diagramas
Para poder conectar la api web con los actuadores utilizamos un Arduino Mega 2560 y una placa creada por el equipo de hardware. 

Esta placa incorpora todos los drivers e indicadores necesarios para el funcionamiento del laboratorio.

**Arduino Mega 2560: PinOut**
<img alt = "Arduino Mega Pro" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistemasDig/main/Imagenes/Arduino-Mega-Pinout.png">

||**ESQUEMÁTICO**||
|-|-|-|
||![Esquematico](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/Esquematico-2.png)||

||**PCB**||
|-|-|-|
||![Pcb Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/Pcb_virtual.png)||
||![Pcb Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/PCB_Virtual-2.png)||
||![Pcb Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/board/Placa-back.jpg)||
||![Pcb Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/board/Placa-frente_1.jpg)||
||![Pcb Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/board/Componentes_3.jpg)||

**Diagrama general**

![Vista General Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_general.png)

**Diagrama Vista superior**

![Vista Superior Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_superior.png)


**Diagrama Vista frontal**

![Vista Frontal Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_frontal.png)


**Datasheets Actuadores**
- [Recopilación de Datasheets](https://renzovigiani.notion.site/adea4f735c994faaa2f919a7e5105e9d?v=83070c8c05d34c02905fc370e37b802a)
- [Servo Motor MG966R](https://github.com/RenzoVigiani/LabRem-Fisica/blob/main/datasheets/MG996R-Datasheet.pdf)
- [Motor Paso a paso](https://github.com/RenzoVigiani/LabRem-Fisica/blob/main/datasheets/MG996R-Datasheet.pdf)
- [Lampara Led](http://wayjun.com/Datasheet/Led/3W%20High%20Power%20LED.pdf)

**Soportes 3D**

 A continuación se pueden ver los elementos diseñados por el equipo de hardware y luego creados a partir de una impresora 3D. Esto permitió generar soportes y gabinetes de acuerdo a los requerimientos y logrando mayor flexibilidad.

Se pueden ver los soportes creados para el sistema de movimiento de todo el ensayo. Se muestran:
- 4 bases para las lentes y el movimiento sobre los rieles.
- 3 soportes para motores paso a paso.
- 1 gabinete para soporte de foco o fuente de luz.
- 1 soporte para la pantalla.

|            |   |
|-------------------------|---------|
| ![Soporte 3D Lente Divergente](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte_3D_Lente_Div.png)  |    ![Soporte 3D Lente Divergente Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte_3D_Lente_Div_real.jpg)|
| ![Soporte 3D Lente Diafragma](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/skp_diseños_2.png)|![Soporte 3D Lente Diafragma Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte_3D_Lente_Diafragma_real..jpg)|
| ![Caja Lampara Led](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/skp_diseños_3.png)  | ![Caja Lampara Led 2](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Caja_3.jpg)  |
| ![Caja Lampara Led Ensamblado](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte+caja_1.jpg) | ![Caja Lampara Led Ensamblado 2](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte+caja_2.jpg) |
| ![Soporte motor paso a paso](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_3.jpg) | ![Soporte motor paso a paso](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_2.jpg) |
| ![Soporte motor paso a paso x3](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_4.jpg) | ![Soporte motor paso a paso x3](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_5.jpg) |




