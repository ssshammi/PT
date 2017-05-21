About The Project:

The aim of this project is to demonstrate the use of direct x and create a procedural environment in 3D. The entire system includes implementation of the lights (directional, diffused, ambient and point lights), importing models(obj), creating meshes (terrains), implementing collisions and player movement. In addition, the engine also has functionalities to create gameobjects and gather inputs from the keyboard and mouse to make a functional prototype.
More details of the generation process can be found in the PDF File below.


Description:

The final project consists of a scene which loads into a dungeon generated with different rooms and connecting corridors. The player is in control of a cube model which serves as a player character for the project. The scene is in a top-down perspective and the player is always visible in the center of the scene. The player emits a light of his own which makes only the environment around the player visible.
The player collides with the sides of the dungeon walls and moves on the surface of the terrain. All the collectables in the scene are also lit up and the player can collect them while moving around.
The terrain is altered just before the generation of the dungeon. The terrain runs multiple algorithms to finally achieve a structure that resembles a smooth hilly region. (Faulting, Perlin Noise, Randomized Noise, Smoothening)
The terrain shader of the dungeon is also modified to have 3 different textures and a warm look to it. The rocks and the slopes are a different texture than that of the floor. There are patches of cracks and dried blood in various regions of the dungeon.
Along with that, the project also has implemented a post process effect of a radial blur and bloom on specific objects.
