include <common.scad>

module base_2d() {
        translate([-d - pcb_margin, -d - pcb_margin])
            square([pcb_x + 2 * pcb_margin + 2 * d, pcb_y + 2 * pcb_margin + 2 * d]);
}

module outer_frame_2d() {
    difference() {
        base_2d();
        translate([-pcb_margin, -pcb_margin])
            square([pcb_x + 2 * pcb_margin, pcb_y + 2 * pcb_margin]);
    }
}

module motor_tabs() {  // in yz plane
    for (i=[0, 1, 2])
        translate([-d-pcb_margin, motor_port_y[i] - motor_width / 2 + pcb_margin, -pcb_thickness])
            cube([d, motor_width - 2 * pcb_margin, pcb_thickness]);
}

module m3_drills() {
    head_height = m3_long_length - thickness_below_pcb - pcb_thickness;
    echo(head_height);
    for (i=[0, 1, 2, 3]) 
        translate([m3_x[i], m3_y[i], 0])
            union() {
                cylinder(d=m3_passthrough_diameter, h=top_height);
                translate([0, 0, head_height])
                    cylinder(d=m3_head_diameter, h=20);
            }
}


module filling() {
    translate([-pcb_margin, -pcb_margin, top_empty_height])
        cube([pcb_x + 2 * pcb_margin, pcb_y + 2 * pcb_margin, top_height - top_empty_height]);
}

module inside_hollows() {
    barrel_d = 11.5;
    barrel_h = top_height - d;
    barrels_x = [5.75, 5.75, 21.5, 86.0, 42.85];
    barrels_y = [46.5, 91.5, 20.75, 36.25, 38.5];
    union() {
        for (i=[0, 1, 2, 3, 4]) 
            translate([barrels_x[i], barrels_y[i], 0])
                cylinder(d=barrel_d, h=barrel_h);
        translate([barrels_x[2] - barrel_d / 2, 0, 0])
            cube([barrel_d, barrels_y[2] , barrel_h]);
    }
}

module pcb_columns() {
    column_d = 4.0 + 2 * pcb_margin;
    column_x = [90.0, 90.0 + d, 90.0, 90.0, 79, 47.5, 62.5, 11,   2,   2,  2,  2, 11, 30.5, 50.5];
    column_y = [28.5, 72.0    , 95.5, 117, 136, 136, 136,   136, 127, 83, 55, 11, 2,   2,    2  ];
    for (i=[0:len(column_x) - 1]) 
        translate([column_x[i], column_y[i], 0])
            linear_extrude(top_height)
                square(column_d, center=true);
}

module jst_cuts() {
    jst_height = 6;
    jst_w = [10.6, 10.6, 10.6, 12.6, 14.6, 16.6, 18.6];
    jst_pos = [40, 55,   70,   126.5, 106, 83.5, 59];
    union() {
        for(i=[0, 1, 2]) 
            translate([jst_pos[i] - jst_w[i] / 2, pcb_y, 0])
                cube([jst_w[i], d + pcb_margin + 1, jst_height]);
        for(i=[3, 4, 5, 6]) 
            translate([pcb_x, jst_pos[i] - jst_w[i] / 2, 0])
                cube([d + pcb_margin + 1, jst_w[i], jst_height]);
    }
}

module edge_walls() {
    linear_extrude(top_height)
        polygon([
            [-pcb_margin - d, -pcb_margin - d],
            [-pcb_margin - d, pcb_y + pcb_margin + d],
            [pcb_x + pcb_margin + d, pcb_y + pcb_margin + d],
            [pcb_x + pcb_margin + d, -pcb_margin - d],
            [-pcb_margin, -pcb_margin - d],
            [-pcb_margin, -pcb_margin],
            [pcb_x + pcb_margin, -pcb_margin],
            [pcb_x + pcb_margin, pcb_y + pcb_margin],
            [-pcb_margin, pcb_y + pcb_margin],
            [-pcb_margin, -pcb_margin - d]
        ]);
}

module button_cuts() {
    button_x = [16.75, 16.75, 16.75, 25.75, 25.75, 25.75];  // new
    button_y = [132.5, 123.5, 114.5, 132.5, 123.5, 114.5];  // new
    //button_x = [16.5, 16.5, 16.5, 26.0, 26.0, 26.0];  // prototype
    //button_y = [132.025, 121.475, 111, 132.025, 121.475, 111];  // prototype
    button_d = 3.9;
    button_cone_height = 1;
    button_cone_base_d = 7.5;
    for (i=[0:len(button_x)-1]) 
        translate([button_x[i], button_y[i], 0])
            union() {
                cylinder(d=button_d, h=top_height);
                translate([0, 0, top_height])
                    scale([1, 1, -1])
                        cylinder(d1=button_cone_base_d, d2=button_d, h=button_cone_height);
    }
}

module encoder_cuts() {
    encoder_x = 60.505;
    encoder_y = [67, 94.5, 122.5];  // new
    //encoder_y = [66, 95, 124.5];  // prototype
    
    encoder_w = 31.5;
    encoder_h = 12.5;
    encoder_wide_h = 23.5;
    encoder_wide_w = 56;
    encoder_wide_base_height = 10.2; // measurement: 10.46
    encoder_min_base_height = top_empty_height + 1;
    for (i=[0, 1, 2]) 
        translate([encoder_x, encoder_y[i], 0])
            union() {
                translate([-encoder_w / 2, -encoder_h / 2, 0])
                    cube([encoder_w, encoder_h, top_height]);
                rotate(90, [0, 1, 0]) rotate(90, [0, 0, 1])
                    linear_extrude(encoder_wide_w, center=true)
                        polygon([
                        [encoder_h / 2, encoder_min_base_height],
                        [-encoder_h / 2, encoder_min_base_height],
                        [-encoder_wide_h / 2, encoder_wide_base_height],
                        [-encoder_wide_h / 2, top_height],
                        [encoder_wide_h / 2, top_height],
                        [encoder_wide_h / 2, encoder_wide_base_height]
                        ]);
            }
}

module usb_cut() {
    usb_x = 62.885;
    usb_z = 3.4;
    usb_h = 3.6;
    usb_w = 9.3;
    translate([usb_x, 0, usb_z])
        cube([usb_w, 10, usb_h], center=true);
    translate([usb_x, -pcb_margin - 0.8 + 5, 0])
        cube([usb_w, 10, usb_z], center=true);
}

module jack_cut() {
    jack_x = 21.5;
    jack_z = 5.0;
    jack_d = 6.0;
    translate([jack_x, 0, jack_z])
        rotate(90, [1, 0, 0])
            cylinder(d=jack_d, h=10, center=true);
}
    
module corridor_cut() {
    union() {
        translate([corridor_x - corridor_w / 2, corridor_y - corridor_len / 2, 0])
            cube([corridor_w, corridor_len, top_height]);
        
        translate([corridor_x, corridor_y, 0])
            rotate(90, [1, 0, 0])
                linear_extrude(corridor_len, center=true)
                    polygon([
                    [-corridor_w / 2 - pcb_margin - d, top_height],
                    [corridor_w / 2 + pcb_margin + d, top_height],
                    [corridor_w / 2 + pcb_margin + d, fan_height],
                    [corridor_w / 2, fan_height - 1 - pcb_margin],  // slope to avoid support
                    [-corridor_w / 2, fan_height - 1 - pcb_margin], // slope to avoid support
                    [-corridor_w / 2 - pcb_margin - d, fan_height]
                    ]);
        translate([corridor_x - fan_d / 2, corridor_y - corridor_len / 2 - fan_thickness, fan_height])
            cube([fan_d, fan_thickness, top_height - fan_height]);
        air_down_cutout_h = (corridor_y - corridor_len / 2 - fan_thickness) - (corridor_horiz_y - corridor_w / 2);
        translate([corridor_x - fan_d / 2 + d / 2, 
                   corridor_y - corridor_len / 2 - fan_thickness - air_down_cutout_h, 
                   0])
            cube([fan_d - d, air_down_cutout_h, top_height]);
        power_supply_h = 4.0;
        translate([corridor_x, corridor_horiz_y - corridor_w / 2, power_supply_h])
            cube([pcb_x + d + pcb_margin - corridor_x, corridor_w, top_height - d - power_supply_h]);
        translate([pcb_x - dcdc_w, 0, 0])
            cube([dcdc_w, dcdc_len, dcdc_h]);
    }
}

module corridor_top() {
    air_down_cutout_h = (corridor_y - corridor_len / 2 - fan_thickness) - (corridor_horiz_y - corridor_w / 2);
    difference() {
        union() {        
            translate([corridor_x, corridor_y, 0])
                rotate(90, [1, 0, 0])
                    linear_extrude(corridor_len, center=true)
                        polygon([
                        [corridor_w / 2 + d, fan_height + fan_d + d],
                        [corridor_w / 2 + d, fan_height],
                        [corridor_w / 2, fan_height - 1],  // slope to avoid support
                        //[corridor_w / 2, fan_height + fan_d],  // slope to avoid support
                        //[-corridor_w / 2, fan_height + fan_d],  // slope to avoid support
                        [-corridor_w / 2, fan_height - 1], // slope to avoid support
                        [-corridor_w / 2 - d, fan_height],
                        [-corridor_w / 2 - d, fan_height + fan_d + d]
                        ]);
            translate([corridor_x - corridor_w / 2 - d, corridor_y + corridor_len / 2, top_height])
                cube([corridor_w + 2 * d, d, fan_d + d - top_height + fan_height]);
            translate([corridor_x - fan_d / 2 - d, corridor_y - corridor_len / 2 - fan_thickness - d, top_height])
                cube([fan_d + 2 * d, fan_thickness + 2 * d, fan_d - (top_height - fan_height) + d]);
            translate([corridor_x - fan_d / 2 - d / 2, 
                   corridor_y - corridor_len / 2 - fan_thickness - air_down_cutout_h - d, 
                   top_height])
                cube([fan_d + d, air_down_cutout_h + d, fan_d - top_height + fan_height + d]);
        }
        translate([corridor_x - fan_d / 2, corridor_y - corridor_len / 2 - fan_thickness, 0])
            cube([fan_d, fan_thickness, fan_height + fan_d]);
        translate([corridor_x - corridor_w / 2, corridor_y - corridor_len / 2, 0])
            cube([corridor_w, corridor_len, fan_height + fan_d]);
        translate([corridor_x - fan_d / 2 + d / 2, 
               corridor_y - corridor_len / 2 - fan_thickness - air_down_cutout_h, 
               top_height])
            cube([fan_d - d, air_down_cutout_h, fan_d - top_height + fan_height]);
        for(i=[0, 1, 2])
            translate([corridor_x - corridor_w / 2, corridor_y + corridor_len / 2, top_height + i * 5.3])
                cube([corridor_w, d, 4.2]);
    }
}

module base() {
    difference() {
        union() {
            edge_walls();
            pcb_columns();
            filling();
            stitching_shape_base();
            motor_tabs();
        }
        m3_drills();
        button_cuts();
        jst_cuts();
        encoder_cuts();
        usb_cut();
        inside_hollows();
        corridor_cut();
        jack_cut();
    }
}

module base_old() {
    difference() {
        union() {
            linear_extrude(d)
                base_2d();
            linear_extrude(d + thickness_below_pcb)
                pcb_support_2d();
            linear_extrude(d + thickness_below_pcb + pcb_thickness)
                outer_frame_2d();
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
                linear_extrude(10, center=true)
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

base();
translate([-37, 0, -fan_d + top_height - fan_height - d])
corridor_top();























