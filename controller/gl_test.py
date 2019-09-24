#!/usr/bin/python
from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GL import *
import sys
from copter import *

name = "Hello, World"
height = 400
width = 400
rotate = 0
beginx = 0.
beginy = 0.
rotx = 0.
roty = 0.

if (len(sys.argv) < 4):
    print("Usage: control.py <serial device> <gain1> <gain2>")
    sys.exit(1)

try:
    quad = Copter(sys.argv[1])
except CopterException, e:
    print e
    sys.exit(1)

quad.updateGains(float(sys.argv[2]), float(sys.argv[3]))
quad.sendConnect()

def display():
    quad.processCommand()
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    gluLookAt(0,0,30,0,0,0,0,1,0)

    if (quad.connected):
        glRotatef(-quad.sensors.angles[1]*57.295,1,0,0)
        glRotatef(-quad.sensors.angles[2]*57.295,0,1,0)
        glRotatef(-quad.sensors.angles[0]*57.295,0,0,1)
    glCallList(1)
    glutSwapBuffers()
    return

def idle():
    glutPostRedisplay()

def mouse(button,state,x,y):
    global beginx,beginy,rotate
    if button == GLUT_LEFT_BUTTON and state == GLUT_DOWN:
        rotate = 1
        beginx = x
        beginy = y
    if button == GLUT_LEFT_BUTTON and state == GLUT_UP:
        rotate = 0
    return

def motion(x,y):
    global rotx,roty,beginx,beginy,rotate
    if rotate:
        rotx = rotx + (y - beginy)
        roty = roty + (x - beginx)
        beginx = x
        beginy = y
        glutPostRedisplay()
    return

def keyboard(key,x,y):
    quad.sendDisconnect()
    sys.exit(0)
    return

glutInit(name)
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
glutInitWindowSize(height,width)
glutCreateWindow(name)
glClearColor(0.,0.,0.,1.)

# setup display list
glNewList(1,GL_COMPILE)
glPushMatrix()
glutSolidCube(2.)
glPopMatrix()
glPushMatrix()
glTranslatef(0.,0.,3.)
glScalef(1.,1.,3.)
glutSolidCube(1.)
glPopMatrix()
glPushMatrix()
glTranslatef(0.,0.,-3.)
glScalef(1.,1.,3.)
glutSolidCube(1.)
glPopMatrix()
glPushMatrix()
glTranslatef(3.,0.,0.)
glScalef(3.,1.,1.)
glutSolidCube(1.)
glPopMatrix()
glPushMatrix()
glTranslatef(-3.,0.,0.)
glScalef(3.,1.,1.)
glutSolidCube(1.)
glPopMatrix()
glEndList()

#setup lighting
glEnable(GL_CULL_FACE)
glEnable(GL_DEPTH_TEST)
glEnable(GL_LIGHTING)
lightZeroPosition = [10.,4.,10.,1.]
lightZeroColor = [0.8,1.0,0.8,1.0] # greenish
glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition)
glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor)
glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1)
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05)
glEnable(GL_LIGHT0)

#setup cameras
glMatrixMode(GL_PROJECTION)
gluPerspective(40.,1.,1.,40.)
glMatrixMode(GL_MODELVIEW)
gluLookAt(0,0,30,0,0,0,0,1,0)
glPushMatrix()

#setup callbacks
glutDisplayFunc(display)
#glutMouseFunc(mouse)
#glutMotionFunc(motion)
glutKeyboardFunc(keyboard)
glutIdleFunc(idle)


glutMainLoop()
