pcb_x = 92;
pcb_y = 138;
pcb_thickness = 1.55;
pcb_margin = 0.2;
thickness_below_pcb = 21.5;
support_nucleo_pins_h = 1.05;
structure_rails_height = 6.0;
d = 2;
m3_mount_diameter = 9.2;
m3_mount_h = 6; 
m3_drill_diameter = 2.8;

motor_port_y_1 = 24; 
motor_port_y_2 = 69;
motor_port_y_3 = 114;

$fn = 32;
    
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
        translate([-pcb_margin - d, motor_port_y_1 / 2 + motor_port_y_2 / 2, 0])
            rotate(90, [0, 1, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
        translate([-pcb_margin - d, motor_port_y_2 / 2 + motor_port_y_3 / 2, 0])
            rotate(90, [0, 1, 0])
                rotate(45, [0, 0, 1])
                    linear_extrude(d)
                        square(square_d, center=true);
    }
}

//stitching_shape_base();