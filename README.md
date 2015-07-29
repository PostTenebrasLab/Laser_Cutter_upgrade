# Laser_Cutter_upgrade

This project aims to improve a cheap Chinese 40W laser cutter, by making it easier and safer to use. 

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
