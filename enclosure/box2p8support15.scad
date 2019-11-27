//lcd 2.8 box top
$fn = $preview ? 12 : 72;
w=68.4;
l=74;
h=11;

difference() {
polyhedron( points = [ [0,0,0], [l,0,0], [l,w,0], [0,w,0], [l*3/4,0,h], [l*3/4,w,h]], faces =[[0,1,2,3], [0,4,1], [0,3,5,4], [4,5,2,1], [3,2,5] ], convexity = 2);

#translate([20, 5, 0]) rotate(a=[0,-15,0]) cube([34,58.4,10],false);
}