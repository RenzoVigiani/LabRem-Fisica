# Protocolo para los laboratorios de "Física Básica"

## Json a enviar
 
    { "Estado": [0,true,false],"Analogico": [1,0,125,542,2]}

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

- Analogico:[Tipo_Diafragma,Cant_Med,Distancia_FL,Distancia_LP]

Distancia_FL = Foco Lente 
Distancia_LP = Lente Pantalla
Cant_Med = Cantidad de mediciones a realizar. Cada vez que incrementa el valor se envía una muestra de las condiciones actuales y vuelve a condiciones iniciales el sistema.
- Tipo_Diafragma = Indica el tipo de diafragma
  - 0 : Sin diafragma
  - 1 : Chico
  - 2 : Mediano
  - 3 : Grande

***Divergente***

- Analogico:[Tipo_Diafragma,Cant_Med,Distancia_FL1,Distancia_L1L2,Distancia_L2P]

## Valores permitidos

Los valores permitidos en las distancias son:

| Distancia         |Valor mín|Valor máx| Unidad |
| ----------------- |---------|---------|--------|
|FL:Foco Lente      |    0    |   900   |  [mm]  |
|LP:Lente Pantalla  |    0    |   900   |  [mm]  |
|FL1:Foco Lente1    |    0    |   700   |  [mm]  |
|L1L2:Lente1 Lente2 |    0    |   700   |  [mm]  |
|L2P:Lente2 Pantalla|    0    |   900   |  [mm]  |

Máxima cantidad de mediciones = 10 [veces]

## Diagramas

**Arduino Mega Pro**

Pin Out
<img alt = "Arduino Mega Pro" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistemasDig/main/Imagenes/Arduino-Mega-Pro.png">

**Esquematico**
![Lab de Fisica 1](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/Esquematico.png)

**Datasheets**
- [Servo Motor MG966R](https://github.com/RenzoVigiani/LabRem-Fisica/blob/main/datasheets/MG996R-Datasheet.pdf)
- [Motor Paso a paso](https://github.com/RenzoVigiani/LabRem-Fisica/blob/main/datasheets/MG996R-Datasheet.pdf)

**Diagrama general**
![Lab de Fisica 1](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_1.png)

**Diagrama Vista superior**
![Lab de Fisica 2](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_2.png)

**Diagrama Vista frontal**
![Lab de Fisica 3](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_3.png)
