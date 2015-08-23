# Laser_Cutter_upgrade

This project aims to improve a cheap Chinese 40W laser cutter, by making it easier and safer to use. 

## Marlin modifications

### control du Laser

Marlin genere des pulses pour le moteurs. Pour aller d'un endroit à un autre à certaine vitesse, il faut à 1KHz envoyer 
une pulse toute les N ms. Pour chaque axe, la vitesse est différente et les pulses sont plus ou moins espacées. 

En mode continu, on utilise les codes 'M3 S???' et 'M5'; M3 = laser on, S100 = laser intensité 100/256, et 
M5 = laser off.

En mode Raster, on va réutiliser l'axe E. Avec une résolution de 10 pts par mm, E va être executé 10x/mm et les 
intensité vont être selectionnées dans un buffer - pour l'instant les accelerations ne sont pas compensées 
(en passant plus de temps au même endroit quand la vitesse est plus basse).

 
### control de temperature (manage_heater())

#### Water cooling pump

Le principe serait de stopper le laser en cas de surchauffe. Il y a deux thermistors une avant le laser (bed) et 
l'autre après (heater0).

La pompe est contrôlée et accélérée en jouant sur PID_MAX en fonction des termistors. 

##### Water cooling tower fan

Le fan de la water cooling tower démarre quand la tour est chaude.
 
#### Air pump
FAN == air pump (mieux vaut disable auto_fan in Configuration_adv.h) reglage avec fanSpeed 

le fonctionnement de la airpump est proche de celle du FAN gestion par Gcode (M106/107) avec fanSpeed (0..255)


### Emergency Stop

 * install door switch and buton with new alim
  
## TODO

 * install an amplification between due output and laser alim
 * test and improve raster mode

### Filtration system

 * install an extraction for the fumes.

## Done

### cooling system

 * add flow sensor
 * add two temperature sensors, one in the flow sensor (output of laser) another at laser input
 * change the water cooling pump
 
### Laser

 * ~~Check the PWM control of the laser~~
 * Laser control intensity use the DAC0 output of SAM3X8E
 * changed lens
 * installed an air flow for the lens
 * installed an air pump
 * installed two red laser for Z probing (one dot and one cross)
 * Laser alim has been remplaced by a new one
 
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
