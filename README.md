# UE4-Tween-Manager
Simple C++ class for creating a tweener for built-in UE4 types. Currently supports float, FVector &amp; FRotator

## Installation - Import

This is not a plugin but simply 2 header files & 1 implementation file. Either include them with your c++ project settings, or create new native C++ classes with the same names using UE4 Editor and copy & paste the file contents.

## Installation - Edit

However you choose to install, please make sure that the #include files in headers and the cpp file are correct. In case you seperate headers and c++ files as private and public, edit the #include's accordingly.

There is a single edit you need to do in order to use the Shake tweens. In lines starting with 262, 286 and 316 in TweenManager.h file you will see the algorithm uses a **delta** value. You need to provide your own way of getting the delta time here. You can use a Game Instance singleton to get the world and get the delta time, or you can use UGameplayStatistics to get it. If you do not edit this, the shake tweens will assume a delta time of 0.016f (60 frames per second), however it wlll not be frame-rate independent.
