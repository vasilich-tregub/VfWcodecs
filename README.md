# Crossbreeding JPEG XS and Video for Windows

A post on SO 
https://stackoverflow.com/questions/22765194/is-it-possible-to-encode-using-the-mrle-codec-on-video-for-windows-under-windows 
started with the words
> I have an old application that provides videos inside .avi containers using the MRLE codec. The
 code uses the Video for Windows API. This has worked admirably for many years, but I've just
 discovered that my code does not behave correctly on Windows 8.

What happens when you are tasked with writing a VfW codec for JPEG XS (a rather modern JPEG flavor) 
you will see in this repo.

Microsoft guides on VfW API are quite restrained on this matter, forewording every page with 
a deprecation warning. So I authored few samples and hope these will be useful for people 
in similar situation.

For now, I could write JPEGXS-compressed AVI files (see folder SvtJXScodec) that can be read by 
ffmpeg tools, the ffmpeg build enhanced with SVT JPEGXS codec. You can find instructions for adding 
SvtJpegxs to ffmpeg in my repo https://github.com/vasilich-tregub/SVT-JPEG-XS forked from 
OpenVisualCloud's SVT-JPEG-XS repo.

I'm going to continue this work and, of course, add more detailed explanations.

This is a derivative work, all credits are included with borrowed code.