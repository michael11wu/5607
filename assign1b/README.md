# Assignment 1a

### How does the apparent rotation of the scene with respect to the viewpoint change with changes in the direction of the ‘up’ vector?
The up vector will define basically which way is up when you looking. The viewdir basically defines where your head is looking at. The updir vector is like a vector that shoot straight up from you head. So if you define a scene where the updir is 0 1 0. Then defining everything else the same except updir is 0 -1 0, your image would be upside down.

![Cube Normal](cube.jpg) 

![Cube Right](cuberight.jpg) 

![Cube Upside Down](cubeupsidedown.jpg)

From these images, the first one is looking straight at the image. The 2nd image is with the updir as 1 0 0. so if you tilt your head right, you should see exact same image as the normal one. Same goes with third image as the updir is 0 -1 0 so look at it upside down. 

In conclusion, updirection basically tells you which was is up and down in your eye point of view

### How do changes in the field of view settings affect the appearance of the scene in your rendered image?

Lower field of view settings will make images look very close to you since you can't see as much around your peripheral. Higher fov settings make it seem as the object is very far from you since you can see so much in your peripheral. 

![High FOV](fov.jpg)

This image has the eye looking straight up the y axis. The two circles are location down the -z axis. Since the fov is very large, you can the -z axis circle. Objects further from the center of the screen are distorted and look stretched. That is why the circles do not look like circles.

### How can the viewing parameters (e.g. the camera location, field of view settings, …) be adjusted to achieve a less exaggerated vs more exaggerated amount of apparent perspective distortion in your image?

Higher field of view settings can create more perspective distortion. But also too low of an fov can also create distortion as you will have tunnel vision basically and won't be able to see the full image. THe further back your eye is, the further you are away from the objects.