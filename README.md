# Protocolo para los laboratorios de "Física Básica"
El procedimiento de comunicación es el siguiente.
Cada vez que se quiere modificar algun dato en el laboratorio. El Servidor debe enviar un

    POST /HTTP/1.1 + <JSON a enviar>

#### Json a enviar (ejemplo)
 
    { "Estado": [0,true,false],"Analogico": [1,125,542,2]}

Y para obtener avances de los datos o reporte de los datos debe enviar un:

      GET /HTTP/1.1

De esta forma el Arduino responde ante una petición.

#### Json a recibir (ejemplo)
 
    { "Estado": [0,true,false],"Analogico": [1,125,542,2], "Error": "Mensaje de error"}

## Sintaxis

**Estado**
Es un array conformado por 3 elementos en el siguiente orden: [Laboratorio, SubLab, Inicio del experimento]

|Laboratorio  | Sub Laboratorio  | Inicio del experimento | Laboratorio Seleccionado | Estado del experimento|
|-|-----|-----|-----------------|--------|
|3|true |true |Lente Convergente|Inicia  |
|3|true |false|Lente Convergente|Finaliza|
|3|false|true |Lente Divergente |Inicia  |
|3|false|false|Lente Divergente |Finaliza|

**Elementos por Laboratorio**

***Convergente***

- Analogico:[Tipo_Diafragma,Distancia_FL,Distancia_LP]

Distancia_FL = Foco Lente 
Distancia_LP = Lente Pantalla
- Tipo_Diafragma = Indica el tipo de diafragma
  - 0 : Sin diafragma
  - 1 : Chico
  - 2 : Mediano
  - 3 : Grande

***Divergente***

- Analogico:[Tipo_Diafragma,Distancia_FL1,Distancia_L2P,Distancia_L1L2]

## Valores permitidos

Los valores permitidos en las distancias son:

| Distancia         |Valor mín|Valor máx| Unidad |
| ----------------- |---------|---------|--------|
|FL:Foco Lente      |    0    |   900   |  [mm]  |
|LP:Lente Pantalla  |    0    |   900   |  [mm]  |
|FL1:Foco Lente1    |    0    |   700   |  [mm]  |
|L1L2:Lente1 Lente2 |    0    |   700   |  [mm]  |
|L2P:Lente2 Pantalla|    0    |   900   |  [mm]  |


**Error**
Es una variable numerica que representa un mensaje de error. 

| Tipo de error                     |  Valor  |
|-----------------------------------|---------|
| Sin errores                       |    0    |
| Error de distancia                |    1    |
| Error de tipo de diafragma        |    2    |
| Error de laboratorio incorrecto   |    3    |

## Diagramas

**Arduino Mega 2560**

Pin Out
<img alt = "Arduino Mega Pro" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistemasDig/main/Imagenes/Arduino-Mega-Pinout.png">


**Arduino Mega Pro**

Pin Out
<img alt = "Arduino Mega Pro" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistemasDig/main/Imagenes/Arduino-Mega-Pro.png">

**Esquematico**

![Esquematico](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/Esquematico.png)

**PCB**
|||
|-|-|
|![Pcb Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/Pcb_virtual.png)|![Pcb Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/board/Placa-back.jpg)|
|![Pcb Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/board/Placa-frente_1.jpg)|![Pcb Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/board/Componentes_3.jpg)|



**Datasheets**
- [Servo Motor MG966R](https://github.com/RenzoVigiani/LabRem-Fisica/blob/main/datasheets/MG996R-Datasheet.pdf)
- [Motor Paso a paso](https://github.com/RenzoVigiani/LabRem-Fisica/blob/main/datasheets/MG996R-Datasheet.pdf)

**Diagrama general**

![Vista General Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_general.png)

**Diagrama Vista superior**

![Vista Superior Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_superior.png)

![Vista Superior Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_distancias.png)



**Diagrama Vista frontal**

![Vista Frontal Virtual](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_virtual_frontal.png)

**Soportes 3D**
|            |   |
|-------------------------|---------|
| ![Soporte 3D Lente Divergente](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte_3D_Lente_Div.png)  |    ![Soporte 3D Lente Divergente Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte_3D_Lente_Div_real.jpg)|
| ![Soporte 3D Lente Diafragma](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/skp_diseños_2.png)|![Soporte 3D Lente Diafragma Real](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte_3D_Lente_Diafragma_real..jpg)|
| ![Caja Lampara Led](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/skp_diseños_3.png)  | ![Caja Lampara Led 2](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Caja_3.jpg)  |
| ![Caja Lampara Led Ensamblado](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte+caja_1.jpg) | ![Caja Lampara Led Ensamblado 2](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/Soporte+caja_2.jpg) |
| ![Soporte motor paso a paso](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_3.jpg) | ![Soporte motor paso a paso](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_2.jpg) |
| ![Soporte motor paso a paso x3](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_4.jpg) | ![Soporte motor paso a paso x3](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/design_3D/pap_5.jpg) |




