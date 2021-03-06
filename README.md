                  +----------------------+
                  |    video_annotator   |
                  +----------------------+

[![Build Status](https://travis-ci.org/arnaud-ramey/video_annotator.svg)](https://travis-ci.org/arnaud-ramey/video_annotator)

Human Action Recognition in realistic scenarios

License :                  see the LICENSE file.
Authors :                  see the AUTHORS file.
How to build the program:  see the INSTALL file.

A GUI tool to read videos and create the annotations on them. The
annotations are defined with user inputs at some keyframes, while they are
interpolated between these keyframes. It allows a fast annotation process,
while being accurate and not too tedious.

Some of the action classes may be context-free which means that inclusion of
the scene in which the action is performed will bring no extra information
about type of the action class. This could be due to two different reasons:
The ﬁrst possible reason is vast diversity of the context in which the
action can be performed; for example sit-down action could be done within
very many different scene contexts. Second reason could be the fact that for
the context-free actions the video frame is almost completely ﬁlled by the
actor and there is not much part of the scene context visible; for example
the kissing action is most likely videotaped in a close frame view and it is
expected to contain not much part of the scene. Nonetheless, it is generally
expected to have a strong correlation between type of the scene in which the
action is performed and the action itself. For example sit-up action most
likely happens in a bedroom scene context. Therefore, generally speaking, it
is helpful to use scene features beside motion features in order to boost up
accuracy of action recognition methods. In this project we will basically
try to incorporate scene features with motion features and study the mutual
effect of these two types of features.

________________________________________________________________________________

How to use the program
________________________________________________________________________________
To display the help, just launch the program in a terminal.
It will display the help of the program.

Synopsis:./build/test.exe VIDEOFILE

Use:
Go to the frame you want to annotate,
select the action you want in the toolbar,and draw a rectangle in the image
This creates a keyframe.
Action ROIs (rectangles) are interpolated between keyframes.

Keys:
 * 'o': move to previous frame
 * 'n': move to next     frame
 * 'l': move to previous keyframe
 * 'm': move to next     keyframe
