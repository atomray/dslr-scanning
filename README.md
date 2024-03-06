# dslr-scanning

## Motivation

Long story short - I recently picked up a medium format film camera and discovered that scanning the negatives
is not as straightforward as I had hoped.
While researching I discovered that many people use old DSLRcameras to capture a digital image of the negative.
And some take multiple images and stitch them together which, given what I was working with, I decided to experiment with.
As it is tedious I thought it would be a good opportunity to learn a bit about Arduino and automate parts of the process.

This respository contains notes and code and will be updated as I progress. It is definitely a work in progress.

**Important Note:** As I am just learning and unfamiliar with many of the concepts I'm bound to misuse words, terms,
and just generally make no sense. I'm interested in corrections and feedback @ atomray420 @ pm . me. Or fork and
create a PR, of course!

## Overview

### Components

#### Camera

I have an Olympus E-510 (circa 2007), a 10 megapixel DSLR, with an Olympus Zuiko 50mm f/2 ED Macro Lens.
An interesting thing about this camera is that it has an IR receiver which is perfect for this project as
I can trigger the camera using an IR LED on the Arduino board.

#### X-Y Plane Table

Had a buddy print up two of  these from me:
[Linear Ball Bearing Stepper Slide](https://www.thingiverse.com/thing:2812734)

One attaches on top of the other. Designed for the stepper motor that comes with the Elegoo Starter Kit.

#### Arduino Components

Bought the Elegoo Super Starter Kit with the UNO R3 and started following the tutorials from Paul McWhorter
(Top Tech Boy?):
[https://www.youtube.com/playlist?list=PLGs0VKk2DiYw-L-RibttcvK-WBZm8WLEP](New Arduino Tutorials)

#### Arduino Program

Code is in this respository. No effort so far to structure code.
