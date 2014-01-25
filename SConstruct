#
# Build file for View3D
#
src = ['ctrans.c','getdat.c','heap.c','misc.c','polygn.c','readvf.c',
       'savevf.c','test3d.c','v3main.c','view3d.c','viewobs.c',
       'viewpp.c','viewunob.c']
env = Environment()
if env['CC'] == 'cl':
    env['CCFLAGS'] = ['/Wall']
    env['CPPDEFINES'] = ['_CRT_SECURE_NO_DEPRECATE']
env.Program('View3D',src)
