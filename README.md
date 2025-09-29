## LICENSE and CREDITS

This is a derivative work, all credits are included with borrowed code.

# Crossbreeding JPEG XS and Video for Windows

A post on SO 
https://stackoverflow.com/questions/22765194/is-it-possible-to-encode-using-the-mrle-codec-on-video-for-windows-under-windows 
started with the words
> I have an old application that provides videos inside .avi containers using the MRLE codec. The
 code uses the Video for Windows API. This has worked admirably for many years, but I've just
 discovered that my code does not behave correctly on Windows 8.

You will see in this repo what happens when you are tasked with writing a VfW codec for JPEG XS, 
a rather modern JPEG flavor.

Microsoft guides on VfW API are quite restrained on this matter, forewording every page with 
a deprecation warning. So I authored few samples and hope these will be useful for people 
in similar situation.

For now, I could write JPEGXS-compressed AVI files (see folder SvtJXScodec) that can be read by 
ffmpeg tools, the ffmpeg build enhanced with an SVT JPEGXS codec. You can find instructions for 
adding SvtJpegxs to ffmpeg in my repo https://github.com/vasilich-tregub/SVT-JPEG-XS forked from 
OpenVisualCloud's SVT-JPEG-XS repo.

I started this work while being rather sceptical about its practical usefulness. But, while doing 
debugging, I was examining avi files produced by both ffmpeg and with VfW AVI. An ffmpeg-produced 
avi file is typically rather ugly clutter of headers and chunks, some avi chunks are empty blocks 
(JUNK) devoid of useful data. The two-character code in data chunk identifiers of video stream 
differs for compressed ('dc') and uncompressed ('db') files, and this difference seems to be 
unnecessary, because the stream header also notifies whether data is compressed. Anyway, 
VfW-produced AVI files use 'db' in data chunk identifiers for both compressed and uncompressed 
files, and this approach does not confuse ffmpeg tools working on VfW-produced files.

While these differences may seem not essential and, admittingly, use of 'dc' marks can become 
necessary in some scenarios, at the first glance VfW-produced AVI files and streams appear 
to be more streamlined.

Another undisputed use of VfW AVI is educational. It may become a nice introduction to Media 
Foundation and ffmpeg frameworks.

