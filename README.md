## Protocolo

En este repositorio se realizarán los cambios a tener en cuenta para el avance de los lab remotos.

#### Json a enviar
 
 {
 "Estado": [ 0, true, false],
 "Llaves": [false],
 "Analogico": [ 1, 0, 125, 542 ]
 }

**Sintaxis**

#### Estado: [Laboratorio, SubLab, Inicio del experimento]

|Laboratorio  | Sub Laboratorio  | Estado del Experimento |
| ------------ | ------------ | ------------ |
| 3  | true  | Fisica Basica: Lente Convergente  |
| 3  | false  | Fisica Basica: Lente Divergente  |

**Inicio del experimento:**

- true: Inicia el experimento
- false: Finaliza el experimento

#### Elementos por Laboratorio

**Fisica Basica**

- Convergente
  - Llaves:[Diafragma]
  - Analogico:[Distancia_FL, Distancia_LP, Cant_Med]
- Divergente
  - Analogico:[Distancia_FL1, Distancia_L1L2, Distancia_L2P, Cant_Med]

**Diagrama general**

![Lab de Fisica 1](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_1.png)

**Diagrama Vista superior**

![Lab de Fisica 2](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_2.png)

**Diagrama Vista frontal**

![Lab de Fisica 3](https://raw.githubusercontent.com/RenzoVigiani/LabRem-Fisica/main/Imagenes/diagrama_3.png)
