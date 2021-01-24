# JPSPlus_Pathfinding
Research project and implementation of JPS+

## Introduction
JPS+ is an improvement on A* and JPS (jump point-search), it runs over a hundred times faster than A*. It's biggest drawback is that it cannot be used when the terrain changes, it does a lot of it's calculations beforehand so that it can be faster at runtime. It's also important to note that JPS+ needs to always have the same cost for the same distance.

While preparing the algorithm burns in jump points and directions to walls, it also shaves off a lot of equivalent routes. This makes it so that it doesn't have to compute multiple times just to have a different route with the same result. 
![Figure1]

If in the above figure a path encounters a wall then the path next to it will split so that all nodes are covered.
![Figure4]

The points from which these optimal paths are pre-calculated are called jump points, these are in spots where you have to travel through for an optimal route. These are stored in the open list, which only contains key points in JPS+ instead of almost every point like in A*. The openlist is a big bottleneck in A* so this is a big advantage.

![Figure2]

JPS+ basically marks chokepoints (you have to pass this node if you want the most optimal path to multiple locations) and has predefined optimal routes to every node in line of sight of the chokepoints. The drawback is that the level has to be static and that distance has to give weight to connections between nodes.

## Implementation
I started with the framework from this course and set everything ready to go, there's even a class called JPSPlus, but it contains AStar since I had no time to start on it.
Here's a cool picture of what it could look like though :).
![Figure3]

## Useful Links
[GameAIPro](http://www.gameaipro.com/)

[JPS+(pdf-file)](http://www.gameaipro.com/GameAIPro2/GameAIPro2_Chapter14_JPS_Plus_An_Extreme_A_Star_Speed_Optimization_for_Static_Uniform_Cost_Grids.pdf) 






[Figure1]: Figures/Figure1.PNG "Nodes are not visited multiple times"
[Figure2]: Figures/Figure2.PNG "Open list has way less nodes with JPS+"
[Figure3]: Figures/Figure3.PNG "The framework I used"
[Figure4]: Figures/Figure4.PNG "When paths get stopped by walls, others split to make sure all nodes are connected"
