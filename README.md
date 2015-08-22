# Laser_Cutter_upgrade

This project aims to improve a cheap Chinese 40W laser cutter, by making it easier and safer to use. 

## Marlin modifications

### control du Laser

Marlin genere des pulses pour le moteurs. Pour aller d'un endroit à un autre à telle vitesse, il faut à 1KHz envoyer 
une pulse toute les N ms. Pour chaque axe, la vitesse est différente et les pulses ont des longueurs différentes. 

Pour le laser on a aussi besoin de pulse. En mode continu, on a besoin d'une pulse d'une durée egale au temps 
de déplacement, pour le mode pulsé, on veut que le laser pulse à une certaine fréquence. Pour faire un déplacement 
d = sqrt(dx²+dy²+dz²) il faut un certain temps et donc faire N pulses.

L'axe E fait ça très bien. Marlin interprète E10 comme devant générer 10 pulses durant ce mouvement.

#### modifications 

Marlin utilise des accelerations et la seul chose à modifier est qu'en accélerant il faut ralentir les pulses 
(pour qu'elles gardent la même fréquence) et vis et versa.

  * il faut modifier les block dans planner; en mode continu E = 1 && mode pulse E = nb
  * il faut aussi tenir compte des pulses incompletes dans une succession de block courts ça peut commencer
    à poser des problèmes d'arrondi . On peut choisir un E > nb de pulse (une résolution 100x plus 
    grande p.ex 0.32p + 5p + 0.47p) Et donc modifier les pulses p.ex; 32H + 100L + 5x (100H + 100B) + 47H.
    Il faut aussi gérer les restes p.ex le block suivant 53H + 100L + ...
 
 
### control de temperature (manage_heater())

Le principe serait de stopper le laser en cas de surchauffe. Il y a deux thermistors une avant le laser (bed) et 
l'autre après (heater0). Pourquoi pas allumer aussi le ventil du watercooling avec heater_1+term_1 quand la tour est
chaude ?

HEATER == watercooling pump
reglage en jouant sur les target_temp du heater
on peut aussi varier la tension sur la pompe en jouant sur PID_MAX

la pompe peut être gérée comme un heater tant que la target température n'est pas atteinte bang-bang (disable PID). 
Dès qu'elle est atteinte, on kill()

On devrait pouvoir modifier facilement le comportement dans température.


FAN == air pump (mieux vaut disable auto_fan in Configuration_adv.h)
reglage avec fanSpeed 

le fonctionnement de la airpump est proche de celle du FAN gestion par Gcode (M106/107) avec fanSpeed (0..255 attention
Due utilise 12bits 0..4095)


### Emergency Stop


  
## TODO

### cooling system

 * change the pump

### Laser

 * check the PWM control of the laser

### Filtration system

 * install an extraction for the fumes.

## Done

### cooling system

 * add flow sensor
 * add two temperature sensors, one in the flow sensor (output of laser) another at laser input
 
### Laser

 * changed lens
 * installed an air flow for the lens
 * installed an air pump
 * installed two red laser for Z probing
 
### controller

 * RAMP-FD[1] (3.3V) installed
 * tested it with Udoo[2] work fine
 
### z axis

 * add an endstop
 * improve friction of the mechanism
 * screw it in place
 * decouper et installer le nid-d'abeille

### Docs link

 * http://www.atmel.com/Images/Atmel-11057-32-bit-Cortex-M3-Microcontroller-SAM3X-SAM3A_Datasheet.pdf
 * http://www.atmel.com/Images/Atmel-42293-SAM4-Digital-to-Analog-Converter-Controller_ApplicationNote_AT07900.pdf
 * http://udoo.org/download/files/schematics/UDOO_REV_D_schematics.pdf

### Links

[1] http://reprap.org/wiki/RAMPS-FD
[2] http://www.udoo.org/docs/

 * https://hackaday.com/2014/08/13/upgrade-that-cheap-o-laser-cutter/
 * https://github.com/lansing-makers-network/buildlog-lasercutter-marlin
 * https://github.com/lansing-makers-network/thlaser-inkscape-plugin

 * http://3dprintzothar.blogspot.ca/2014/08/40-watt-chinese-co2-laser-upgrade-with.html
 * http://www.lightobject.com/Power-table-bed-kit-for-K40-small-laser-machine-P722.aspx

 Marlin on arduino Due (3d printer)
 * https://github.com/bobc/Marlin
