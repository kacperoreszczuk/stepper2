include <top.scad>
include <bottom.scad>


base_bottom();
translate([0, 0, d + thickness_below_pcb + pcb_thickness + 0.3])
    union () {
        base();
        corridor_top();
    }