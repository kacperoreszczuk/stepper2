pcb_x = 92;
pcb_y = 138;
pcb_thickness = 1.55;
pcb_margin = 0.15;
thickness_below_pcb = 21.5;
support_nucleo_pins_h = 1.05;
structure_rails_height = 6.0;
d = 1.5;
m3_mount_diameter = 9.2;
m3_mount_h = 6; 
m3_drill_diameter = 2.8;
m3_passthrough_diameter = 3.2;
m3_head_h = 3.0;
m3_head_diameter = 5.8;
m3_long_length = 30.6; // M3x30 with additional margin

top_height = 13;
top_empty_height = 6.3;

motor_port_y = [24, 69, 114];
motor_width = 32.5;

m3_x = [5, 5, 46.98, 80.00];
m3_y = [5, 133, 80.86, 28.79];
$fn = 32;

corridor_x = 21.5;
corridor_y = 68.5;
corridor_w = 16;
corridor_len = 62.0;
corridor_horiz_y = 12.89;

fan_d = 20.3;
fan_thickness = 10.7;
fan_height = 7.5;
    
module stitching_shape_base() {
    square_d = sqrt(2) * pcb_thickness;
    union() {
        translate([10, -pcb_margin - d, 0])
            rotate(-90, [1, 0, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
        translate([10, pcb_y + pcb_margin, 0])
            rotate(-90, [1, 0, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
        translate([pcb_x + pcb_margin, 43, 0])
            rotate(90, [0, 1, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
        translate([-pcb_margin - d, motor_port_y[0] / 2 + motor_port_y[1] / 2, 0])
            rotate(90, [0, 1, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
        translate([-pcb_margin - d, motor_port_y[1] / 2 + motor_port_y[2] / 2, 0])
            rotate(90, [0, 1, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
    }
}

//stitching_shape_base();