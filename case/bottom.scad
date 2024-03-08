include <common.scad>

module pcb_support_2d() {
    left_square_pad_size = 8.5;
    left_midpoint_y_1 = 46.5;
    left_midpoint_y_2 = 91.5;
    top_left_bar_len = 36;
    top_right_bar_len = 16;
    top_bar_width = 3;
    right_bar_width = 0.5;
    bottom_left_bar_len = 20;
    bottom_left_bar_width = 3;
    bottom_right_bar_len = 45;
    bottom_right_bar_width = 1.2;
    union() {
        
        translate ([-pcb_margin, -pcb_margin]) 
            square(pcb_margin + left_square_pad_size);
        translate ([-pcb_margin, pcb_y - left_square_pad_size]) 
            square(pcb_margin + left_square_pad_size);
        translate ([-pcb_margin, left_midpoint_y_1 - left_square_pad_size / 2]) 
            square([pcb_margin + left_square_pad_size, left_square_pad_size]);
        translate ([-pcb_margin, left_midpoint_y_2 - left_square_pad_size / 2]) 
            square([pcb_margin + left_square_pad_size, left_square_pad_size]);
        
        translate ([-pcb_margin, pcb_y - top_bar_width]) 
            square([pcb_margin + top_left_bar_len, pcb_margin + top_bar_width]);
        translate ([pcb_x - top_right_bar_len, pcb_y - top_bar_width]) 
            square([pcb_margin + top_right_bar_len, pcb_margin + top_bar_width]);
        
        translate ([pcb_x - right_bar_width, -pcb_margin]) 
            square([pcb_margin + right_bar_width, pcb_y + 2 * pcb_margin]);
        
        
        translate ([-pcb_margin, -pcb_margin]) 
            square([pcb_margin + bottom_left_bar_len, pcb_margin + bottom_left_bar_width]);
        translate ([pcb_x - bottom_right_bar_len, -pcb_margin]) 
            square([pcb_margin + bottom_right_bar_len, pcb_margin + bottom_right_bar_width]);
    }
}

module base_2d_bottom() {
    translate([-d - pcb_margin, -d - pcb_margin])
        square([pcb_x + 2 * pcb_margin + 2 * d, pcb_y + 2 * pcb_margin + 2 * d]);
}

module outer_frame_2d_bottom() {
    difference() {
        base_2d_bottom();
        translate([-pcb_margin, -pcb_margin])
            square([pcb_x + 2 * pcb_margin, pcb_y + 2 * pcb_margin]);
    }
}

module structure_rails_2d() {
    rail_vertical_x_1 = 44;
    rail_vertical_x_2 = 50;
    rail_horizontal_y_1 = 26;
    rail_horizontal_y_2 = 105;
    union() {
        translate([-pcb_margin, rail_horizontal_y_1 - d / 2])
            square([pcb_x + 2 * pcb_margin, d]);
        translate([-pcb_margin, rail_horizontal_y_2 - d / 2])
            square([pcb_x + 2 * pcb_margin, d]);
        translate([rail_vertical_x_1 - d / 2, -pcb_margin])
            square([d, pcb_y + 2 * pcb_margin]);
        translate([rail_vertical_x_2 - d / 2, -pcb_margin])
            square([d, pcb_y + 2 * pcb_margin]);
    }
}

module support_nucleo_pins_2d() {
    x1 = 30.47;
    y1 = 32.6;
    w1 = 5.08;
    h1 = 25.4;
    
    x2 = 30.47;
    y2 = 59.27;
    w2 = 5.08;
    h2 = 43.18;
    
    x3 = 76.19;
    y3 = 41.49;
    w3 = 5.08;
    h3 = 20.33;
    
    x4 = 76.19;
    y4 = 64.35;
    w4 = 5.08;
    h4 = 38.10;
    
    
    union() {
        translate([x1, y1])
            square([w1, h1]);
        translate([x2, y2])
            square([w2, h2]);
        translate([x3, y3])
            square([w3, h3]);
        translate([x4, y4])
            square([w4, h4]);
    }
}

module motor_cutouts_2d() {  // in yz plane
    depth = 14.9;
    union() {
        for (i=[0, 1, 2])
        translate([motor_port_y[i] - motor_width / 2, d + thickness_below_pcb + pcb_thickness - depth])
            square([motor_width, depth + 1]);
    }
}

module m3_mount_base() {
    for (i=[2, 3]) 
        translate([m3_x[i], m3_y[i], 0])
            cylinder(d=m3_mount_diameter, h=d+m3_mount_h);
}

module m3_mount_drills() {
    for (i=[0, 1, 2, 3]) 
        translate([m3_x[i], m3_y[i], d])
            cylinder(d=m3_drill_diameter, h=thickness_below_pcb + pcb_thickness);
}

module mount_tab(yyy) {
    head_diameter = 10.2;
    drill_offset = head_diameter / 2 + 7;
    drill_diameter = 6.2;
    base_thickness = 5 + d;
    
   
    translate([-d-pcb_margin, yyy, 0])
        difference() {
            linear_extrude(height=base_thickness) 
                polygon([
                [0, -head_diameter * 1.5], 
                [-drill_offset - head_diameter * 0.8, -head_diameter / 2],
                [-drill_offset - head_diameter * 0.8, head_diameter / 2],
                [0, head_diameter * 1.5]       
            ]);
            translate([-drill_offset, 0, d])
                cylinder(h=100, d=head_diameter);
            translate([-drill_offset, 0, -1])
                cylinder(h=100, d=drill_diameter);
        }
}


module stlink_drill() {
    x = 42;
    depth = 9.5;
    w = 7.5;
    h = 4.0;
    translate([x, -pcb_margin - d / 2, d + thickness_below_pcb - depth])
        cube([w, 10, h], center=true);
}

module ethernet_drill() {
    x = 46.85;
    depth = 9.5;
    w = 16.25;
    h = 12;
    translate([x, pcb_y + pcb_margin + d / 2, d + thickness_below_pcb - depth])
        cube([w, 10, h], center=true);
}

module base_bottom() {
    difference() {
        union() {
            linear_extrude(d)
                base_2d_bottom();
            linear_extrude(d + thickness_below_pcb)
                pcb_support_2d();
            linear_extrude(d + thickness_below_pcb + pcb_thickness)
                outer_frame_2d_bottom();
            linear_extrude(d + structure_rails_height)
                structure_rails_2d();
            linear_extrude(d + support_nucleo_pins_h)
                support_nucleo_pins_2d();
            m3_mount_base();
            mount_tab(pcb_y / 2 - 25);
            mount_tab(pcb_y / 2 + 25);
        }
        rotate(90, [1, 0, 0]) 
            rotate(90, [0, 1, 0])
                linear_extrude(17, center=true)
                    motor_cutouts_2d();
        m3_mount_drills();
        translate([0, 0, d + thickness_below_pcb + pcb_thickness])
            minkowski() {
                stitching_shape_base();
                cube(0.1);
            }
        stlink_drill();
        ethernet_drill();
    }
}

base_bottom();
//structure_rails_2d();























