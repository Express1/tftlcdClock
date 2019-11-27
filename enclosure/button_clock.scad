// button
$fn = $preview ? 12 : 72;
wall =2;
intersection() {
    union() {
    cylinder(d=5, h=2+wall+1);
    cylinder(d=5+3, h=1);
    }
    sphere(r=2+wall+1);
}