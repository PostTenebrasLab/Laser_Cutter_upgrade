# Laser_Cutter_upgrade

This project aims to improve a cheap Chinese 40W laser cutter, by making it easier and safer to use. 

## Marlin modifications

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

### Fonctionnement du Laser  

Dans planner, plan_buffer_line() gère les accélérations et serait un bon candidat pour gérer l'allumage du laser

voir si on peut l'intégrer dans la struct block_t. Marlin traite des blocks dans une FIFO (instruction de mouvement)
pourquoi ne pas en profiter pour intégrer le contrôle du laser ?

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
