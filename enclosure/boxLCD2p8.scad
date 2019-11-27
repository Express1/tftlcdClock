//lcd 2.8 box top
$fn = $preview ? 12 : 72;
//
// inside size
wid1 = 54;
len1= 90+10;
height = 50;
wall = 2;
gap = 0.05;
cornerRadius = 10;

//buttons
 s=2.54;
 g=0.2;
 sy=11;  


//bottom
difference() {
    translate([0, 0, 0]) roundedBox(wid1+2*wall, len1+2*wall, height/2+wall,cornerRadius);
    union() { 
        translate([wall,wall,wall]) roundedBox(wid1, len1, height/2, cornerRadius); 
        translate([wall-1,wall-1,height/2+wall-4-gap]) roundedBox(wid1+2,len1+2,4+gap,cornerRadius);
        //lcd hole
        translate([17+2+wall-0.5,4+wall,0]) cube(size=[61,46,wall]);
        //cuts for buttons    
        translate([len1+wall,sy,wall]) 
            union()
                for (a =[0:2])  
                {
                 #translate([0,0+a*s*5+2*s,wall+12.7]) rotate([0,90,0]) cylinder(d=5.2,h=wall);
                 *translate([0,0+a*s*5,0]) cube(size=[wall,g,6*s]); 
                 *translate([0,4*s+a*s*5,0]) cube(size=[wall,g,6*s]);
                 *translate([0,+a*s*5,6*s]) cube(size=[wall,4*s+g,g]);  
                }
         //buzzer hole
         *translate ([50,wall,23]) rotate(a=[90,0,0]) cylinder(r=1.5,h=2,center=false);  
         // USB hole
         #translate ([18+2+wall,wall,7.3+wall]) rotate(a=[90,0,0]) cube(size=[8,4,wall]);      
        }
    } 



//LCD feet
 translate([9+wall,5+wall,wall]) {
    cylinder(8,d=2.5);
        }
translate([85+wall,5+wall,wall]) {
    cylinder(8,d=2.5);
        }
        translate([9+wall,49+wall,wall]) {
    cylinder(8,d=2.5);
        }
        translate([85+wall,49+wall,wall]) {
    cylinder(8,d=2.5);
        }

// button rails
     
    difference() {
    translate([wall+len1-10,sy-6,wall]) cube(size=[10,6,8*s]);
    translate([wall+len1-10+2,sy-s,wall]) cube(size=[1.8,s,8*s]);
                 }    
         
    difference() {
    translate([wall+len1-10,sy+3*5*s-s,wall]) cube(size=[10,6,8*s]);
    translate([wall+len1-10+2,sy+3*5*s-s,wall]) cube(size=[1.8,s,8*s]);
    }


//buzzer
    *translate ([50,4,23])
    rotate(a=[90,0,0]) {
    difference() {
        cylinder(r=27/2+1,h=2,center=false);
        union() {
            cylinder(r=27/2,h=2,center=false);
            cylinder(r=27/2+0.1,h=0.5,center=false);
            } 
    }
}  




module roundedBox(wid1, len1, height, radius)
{
    dRadius = 2*radius;
    cube(size=[len1,wid1, height]);
  
}


