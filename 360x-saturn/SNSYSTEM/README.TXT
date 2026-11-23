Hi there,
 The problem occurs to the array 'sprites_attribs' in the file PROCESS.C
The array 'particle_list_dx seems to start 256 bytes after the start of
sprites_attribs and thus trashes all the data.
 The value for MAX_PARTICLES is, I think, 512. NAME_END is something similar

 Pat @ NMS Software

