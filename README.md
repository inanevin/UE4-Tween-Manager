# UE4-Tween-Manager
Simple C++ class for creating a tweener for built-in UE4 types. Currently supports float, FVector &amp; FRotator
![image](https://user-images.githubusercontent.com/3519379/123952578-da9a1500-d9ae-11eb-92f0-5d1a7e8ef952.png)

## Installation - Import

This is not a plugin but simply 2 header files & 1 implementation file. Either include them with your c++ project settings, or create new native C++ classes with the same names using UE4 Editor and copy & paste the file contents.

## Installation - Edit

However you choose to install, please make sure that the #include files in headers and the cpp file are correct. In case you seperate headers and c++ files as private and public, edit the #include's accordingly.

There is a single edit you need to do in order to use the Shake tweens. In lines starting with 262, 286 and 316 in TweenManager.h file you will see the algorithm uses a **delta** value. You need to provide your own way of getting the delta time here. You can use a Game Instance singleton to get the world and get the delta time, or you can use UGameplayStatistics to get it. If you do not edit this, the shake tweens will assume a delta time of 0.016f (60 frames per second), however it wlll not be frame-rate independent.

## Usage

For every tween, you need to give it an ID. You can add a new element to the ETweenID enum in TweenCommon.h. After that, use it like this:

```cpp

// Interpolates member variable myFloat from 0 to 10 in 0.5 seconds.
CTweenManager::Get(ETweenID::MyTweenID)->Interpolate(&myFloat, 0.0f, 10.0f, 0.5f);

// Shakes member variable myFloat by the amount of 3.0f with the speed of 12.0f for 2 seconds.
// Pass the duration as -1.0f if you want infinite shake.
CTweenManager::Get(ETweenID::MyTweenID)->Shake(&myFloat, 3.0f, 12.0f, 2.0f);

// You can stop the tweens as such.
CTweenManager::StopTween(ETweenID::MyTweenID);

```

You can set various properties for the tweener:

```cpp

// Interpolates member variable myFloat from 0 to 10 in 0.5 seconds. The interpolation rate is 0.01f, meaning the update function will be called every 0.01f seconds
// resulting in a smoother tween (default 0.02). Sets the tween ease as SineIn function, sets 2 second delay on the tween and tells the tween to remove it from the
// list of tweens after its done. Use it for non-frequent tweens.
CTweenManager::Get(ETweenID::MyTweenID)->SetRate(0.01f).SetEase(ETweenEase::SineIn).SetDelay(2.0f).SetRemoveWhenFinished(true).Interpolate(&myFloat, 0.0f, 10.0f, 0.5f);

// You can bind OnUpdate or OnCompleted delegates
CTweenManager::Get(ETweenID::MyTweenID)->Interpolate(&myFloat, 0.0f, 10.0f, 0.5f).OnComplete().BindRaw.....

// By getting OnComplete(). you can reach the default UE delegate binding functions. You can use the same for OnUpdate().


```
