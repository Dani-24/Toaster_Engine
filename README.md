# Toaster Engine

<div id="header" align="center">
  <img src="https://user-images.githubusercontent.com/79161140/192161224-a374e6b4-c0f1-47d2-9202-6a044b5bb32f.png" width="300"/>
  
  <p>
  <a href="https://github.com/Dani-24/Toaster_Engine/graphs/contributors">
    <img src="https://img.shields.io/github/contributors/Dani-24/Toaster_Engine" alt="contributors" />
  </a>
  <a href="">
    <img src="https://img.shields.io/github/last-commit/Dani-24/Toaster_Engine" alt="last update" />
  </a>
  <a href="https://github.com/Dani-24/Toaster_Engine/network/members">
    <img src="https://img.shields.io/github/forks/Dani-24/Toaster_Engine" alt="forks" />
  </a>
  <a href="https://github.com/Dani-24/Toaster_Engine/stargazers">
    <img src="https://img.shields.io/github/stars/Dani-24/Toaster_Engine" alt="stars" />
  </a>
  <a href="https://github.com/Dani-24/Toaster_Engine/issues/">
    <img src="https://img.shields.io/github/issues/Dani-24/Toaster_Engine" alt="open issues" />
  </a>
</p>
</div>

## Information

Thanks (or sorry) for downloading [Toaster Engine](https://github.com/Dani-24/Toaster_Engine). As the name implies, this engine is like a toaster so it does it job perfectly bad. Lastest release right here -> [Last Toaster Engine Release](https://github.com/Dani-24/Toaster_Engine/releases).

This thing has been developed by Dani Toledo , aka [Dani24](https://github.com/Dani-24) and i added music & skybox to the lastest release bc it's cooler

### Controls

Just use the mouse!

Any specific shortcut is shown at the engine UI.

### Default Animations

By default the engines opens up with an animated moai playing an idle animation.

By pressing "1", the Moai triggers an attack animation.

By pressing "2", the Moai changes its idle animation to in place walk animation until the key is released

### Camera

* Movement : WASD

* Rotation : Left Click

* Zoom : Mouse Wheel

* Pick an item from Editor camera view : Right Click

## Engine : FEATURES

[Toaster Engine](https://github.com/Dani-24/Toaster_Engine) is a simple and easy-to-use game 3d engine written in C++ programming language. It provides a set of functions to create a wide range of 3D games, as well as other multimedia applications. [Toaster Engine](https://github.com/Dani-24/Toaster_Engine) is not designed to be portable, fast, and easy to understand, making it a wrong choice for beginners and experienced programmers alike. The engine is open-source and available under the mit license, allowing developers to use it in commercial and non-commercial projects.

### SKELETAL ANIMATIONS : MAIN FEATURE

Try loading "RussianRoulette.FBX" from the /Assets to see the incredible Bones hierarchy in action. But BE CAREFUL!!! As the name implies, the russian roulette mesh will crash your Toaster randomly if you're not lucky. Just don't give up and keep trying!! I promise you it works!

<p align="center">

  <img align="center" src="https://user-images.githubusercontent.com/79161140/212535270-437bc521-d65a-4625-a572-620a92d246ab.png">

</p>

Then just select the parent of the Skeleton and the Inpector will show you the Animation Component

<p align="center">

  <img align="center" src="https://user-images.githubusercontent.com/79161140/212535341-6dc308ae-2ad0-416f-b130-4aba9a2690af.png">

</p>

### Scene & Editor Views

At the center of the screen you can choose between the Editor & the Scene views, the editor is where you will edit and do all the job, while the scene will be the result processed view of whatever you're doing. The Editor works with an editor camera controlled with the mouse, but the Scene works with Camera GameObjects that you can interact with at the Inspector by selecting them at the Editor.

### Resource Manager & Asset Explorer

<p align="center">

  <img align="center" src="https://user-images.githubusercontent.com/79161140/212535054-baef8c1d-a1d0-4766-854b-49c7c717eb14.png">

</p>

All files located at the engine /Assets folder will be displayed here. 3D meshes and 2d Images can be dragged from here to be applied on the Editor.

What's even better is that you can drag any file from anywhere and Toaster Engine will save a copy of it into /Assets to allow you to use this file easily.

### Inspector

<p align="center">

  <img align="center" src="https://user-images.githubusercontent.com/79161140/212535201-756d7fda-3969-40a2-b4a4-2889a223853e.png">

</p>

Just select or create a GameObject and the Inspector will show you all its propierties

### Fast 3d meshes import with [Assimp](https://github.com/assimp/assimp)

### MousePicking and Frustum Culling features to improve the Toaster experience. Which you can improve by activating the Toaster Mode at the Edit menu from the top-bar menu. 

<p align="center">

  <img align="center" src="https://user-images.githubusercontent.com/79161140/212535534-80ccccd8-e533-419a-9f7a-b4ec04c9c091.png">

</p>

### 1000 Moais

<img align="center" src="https://user-images.githubusercontent.com/79161140/212564399-7dd71a7c-90c5-4ca1-8080-5fb36e4d148f.png">

### And More

Just test all the top-bar menu options to see all the UI features as window options & Engine information and 3rd party software used as [Glew](https://glew.sourceforge.net) [Mmgr](https://github.com/RIscRIpt/mmgr), [GPUDetector](https://www.intel.es/content/www/es/es/homepage.html) and [DevIL](https://openil.sourceforge.net)

Remember to check Window -> Configuration to play with some render functionalities, check the input and the memory use.

## FAQ

### How to Use

1 - Download the lastest release

2 - Unzip the release

3 - Open the unzipped folder and look for the .exe file

4 - Cook your computer CPU for 5 minutes

5 - Enjoy


## 3rd Party Software

[SDL2](https://www.libsdl.org)

[OpenGL](https://www.opengl.org)

[Glew](https://glew.sourceforge.net)

[Assimp](https://github.com/assimp/assimp)

[ImGui](https://github.com/ocornut/imgui)

[Parson](https://github.com/kgabis/parson)

[PhysFS](https://icculus.org/physfs/)

[Mmgr](https://github.com/RIscRIpt/mmgr), or also obviously known as [Mar Menor Golf Resort](https://www.mmgr.info/es/)

[GPUDetector](https://www.intel.es/content/www/es/es/homepage.html)

[DevIL](https://openil.sourceforge.net)

<p align="center">
  
  <img align="center" src="https://user-images.githubusercontent.com/79161140/212534887-5309dbd6-a156-41b7-b3e2-58e245ffbfef.png">
  
</p>

<div id="header" align="center">

## [Licence](https://github.com/Dani-24/Toaster_Engine/blob/main/LICENSE)

</div>
