# JPSPlus_Pathfinding
Research project and implementation of JPS+

## Introduction
JPS+ is an improvement on A* and JPS (jump point-search), it runs over a hundred times faster than A*. It's biggest drawback is that it cannot be used when the terrain changes, it does a lot of it's calculations beforehand so that it can be faster at runtime. It's also important to note that JPS+ needs to always have the same cost for the same distance.

While preparing the algorithm burns in jump points and directions to walls, it also shaves off a lot of equivalent routes. This makes it so that it doesn't have to compute multiple times just to have a different route with the same result. 
![Figure1]


![Figure2]

## Implementation
![Figure3]

## Useful Links
[GameAIPro](http://www.gameaipro.com/)

[JPS+(pdf-file)](http://www.gameaipro.com/GameAIPro2/GameAIPro2_Chapter14_JPS_Plus_An_Extreme_A_Star_Speed_Optimization_for_Static_Uniform_Cost_Grids.pdf) 






[Figure1]: Figures/Figure1.PNG "Nodes are not visited multiple times"
[Figure2]: Figures/Figure2.PNG "Open list has way less nodes with JPS+"
[Figure3]: Figures/Figure3.PNG "The framework I used"
