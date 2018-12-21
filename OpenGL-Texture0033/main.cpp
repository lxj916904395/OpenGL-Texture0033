//
//  main.m
//  OpenGL-Texture0033
//
//  Created by zhongding on 2018/12/21.
//

#include "GLBatch.h"
#include "GLTriangleBatch.h"
#include "GLMatrixStack.h"
#include "GLFrustum.h"
#include "GLFrame.h"
#include "StopWatch.h"
#include "GLShaderManager.h"
#include "GLGeometryTransform.h"


#ifdef __APPLE__
#include <glut/glut.h>
#else
#include <GL/glut.h>
#endif

GLShaderManager shaderManager;
GLFrame cameraFrame;
GLFrustum viewFrustum;

GLMatrixStack modelViewMatri;
GLMatrixStack projectionMatri;

GLGeometryTransform transformLine ;

GLBatch floorBatch;
GLTriangleBatch sunBatch;
GLTriangleBatch earthBatch;
GLTriangleBatch othersBatch;


#define texture_count 3
GLuint textures[texture_count];


void specailKeys(int key , int x,int y){
    if(key == GLUT_KEY_UP)
        cameraFrame.MoveForward(1);
    
    if (key == GLUT_KEY_DOWN)
        cameraFrame.MoveForward(-1);
    
    if (key == GLUT_KEY_LEFT)
        cameraFrame.RotateWorld(m3dDegToRad(5), 0,1,0);
    
    if(key == GLUT_KEY_RIGHT)
        cameraFrame.RotateWorld(m3dDegToRad(-5), 0, 1, 0);
    
    glutPostRedisplay();
}

void changeSize(int w, int h){
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(10, float(w)/float(h), 1, 150);
    
    projectionMatri.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformLine.SetMatrixStacks(modelViewMatri, projectionMatri);
}


bool loadTGAFile(const char *filename,GLenum minFilter,GLenum magFilter, GLenum mode){
    GLbyte *pbyte;
    GLint width, height, components;
    GLenum formatter;
    
    
    pbyte = gltReadTGABits(filename, &width, &height, &components, &formatter);
    
    if (pbyte == NULL){
        printf("读取tga出错");
        return false;
    }
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
    
    glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, formatter, GL_UNSIGNED_BYTE, pbyte);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
    glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
    
    free(pbyte);
    
    return true;
}

void setup(void){
    
    glClearColor(0, 0, 0, 1);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    
    floorBatch.Begin(GL_TRIANGLE_FAN, 4,1);
    floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    floorBatch.Vertex3f(-20.f, -1, 30.0f);
    
    floorBatch.MultiTexCoord2f(0, 1, 0.0f);
    floorBatch.Vertex3f(50.0f,0, -30.f);
    
    floorBatch.MultiTexCoord2f(0, 1, 1);
    floorBatch.Vertex3f(-20.0f, 0, -30.0f);
    
    floorBatch.MultiTexCoord2f(0, 0.0f, 1);
    floorBatch.Vertex3f(50.0f, -1, 30.0f);
    floorBatch.End();
    
    
    gltMakeSphere(sunBatch, 0.4, 30, 30);
    gltMakeSphere(earthBatch, 0.1, 30, 30);
    gltMakeSphere(othersBatch, 0.2, 30, 30);
    
    glGenTextures(texture_count, textures);
    
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    loadTGAFile("Marble.tga", GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE
                );
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    loadTGAFile("son.tga", GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE
                );
    
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    loadTGAFile("Marslike.tga", GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE
                );
    
    cameraFrame.MoveForward(-8);

}

static GLfloat lightPos[] = { 1.0f, 1.50f, 0.0f, 1.0f };
static GLfloat withte[] = { 1.0f, 1.0f, 1.0f, 1.f};
GLfloat floorColor[] = { 1.0f, 1.0f, 0.0f, 0.6f};

void jingxiang(GLfloat yro){
    
    
    M3DVector4f lightTransform;
    M3DMatrix44f mcamera;
    cameraFrame.GetCameraMatrix(mcamera);
    m3dTransformVector4(lightTransform, lightPos, mcamera);
    
    modelViewMatri.PushMatrix();
    modelViewMatri.Translatev(lightTransform);
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformLine.GetModelViewProjectionMatrix(),withte);
    othersBatch.Draw();
    
    modelViewMatri.PopMatrix();
    
    modelViewMatri.Translate(0, 0.05, -3);

    modelViewMatri.PushMatrix();
    modelViewMatri.Rotate(yro, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,modelViewMatri.GetMatrix(),transformLine.GetProjectionMatrix(),lightTransform,withte,0);
    sunBatch.Draw();
    modelViewMatri.PopMatrix();
    
    
    modelViewMatri.PushMatrix();
    modelViewMatri.Rotate(yro*1.5, 0, 1, 0);
    modelViewMatri.Translate(1, 0, 0);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,modelViewMatri.GetMatrix(),transformLine.GetProjectionMatrix(),lightTransform,withte,0);
    earthBatch.Draw();
    modelViewMatri.PopMatrix();
}


void rendScene(void){
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    static CStopWatch timer;
    GLfloat yro = timer.GetElapsedSeconds() * 60;
    
    modelViewMatri.PushMatrix();
    M3DMatrix44f mcamera;
    cameraFrame.GetCameraMatrix(mcamera);
    modelViewMatri.MultMatrix(mcamera);
    
    //镜像
    modelViewMatri.PushMatrix();
    modelViewMatri.Scale(1, -1, 1);
    modelViewMatri.Translate(0, 0.9, 0);
    glFrontFace(GL_CW);
    jingxiang(yro);
    glFrontFace(GL_CCW);
    modelViewMatri.PopMatrix();
    
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,transformLine.GetModelViewProjectionMatrix(),floorColor,0);
    floorBatch.Draw();
    
    glDisable(GL_BLEND);
    
    jingxiang(yro);
    
    modelViewMatri.PopMatrix();
    glutSwapBuffers();
    
    glutPostRedisplay();
}

int main(int argc, char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL综合运用");
    
    glutDisplayFunc(rendScene);
    glutReshapeFunc(changeSize);
    glutSpecialFunc(specailKeys);
    
    GLenum err = glewInit();
    if (err != GLEW_OK){
        return 1;
    }

    setup();
    
    glutMainLoop();
    
    glDeleteTextures(texture_count, textures);
    
    return 0;
}
