#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LIBPNG\png.h>
 

/* Bill: Start Code*/
//	ffmpeg_encoder_start("tmp.mpg", AV_CODEC_ID_MPEG1VIDEO, 25, width, height);
/* Bill: Finish Code*/
//	ffmpeg_encoder_finish();
//	free(rgb);

enum Constants { SCREENSHOT_MAX_FILENAME = 256 };
static GLubyte *pixels = NULL;

/* Framebuffer objects */
static GLuint fbo;
static GLuint rbo_color;
static GLuint rbo_depth;
static int offscreen = 1;
static unsigned int max_nframes = 128;
static unsigned int nframes = 0;
static unsigned int time0;
static unsigned int height = 128;
static unsigned int width = 128;
#define PPM_BIT (1 << 0)
#define LIBPNG_BIT (1 << 1)
#define FFMPEG_BIT (1 << 2)
static unsigned int output_formats = PPM_BIT | LIBPNG_BIT | FFMPEG_BIT;

/* Model. */
static double angle;
static double delta_angle;

static void model_init(void) {
    angle = 0;
    delta_angle = 1;
}

static int model_update(void) {
    angle += delta_angle;
    return 0;
}

static int model_finished(void) {
    return nframes >= max_nframes;
}

static void init(void)  {
    int glget;

    if (offscreen) {
        /*  Framebuffer */
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        /* Color renderbuffer. */
        glGenRenderbuffers(1, &rbo_color);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
        /* Storage must be one of: */
        /* GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX8. */
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);

        /* Depth renderbuffer. */
        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        /* Sanity check. */
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER));
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glget);
        assert(width < (unsigned int)glget);
        assert(height < (unsigned int)glget);
    } else {
        glReadBuffer(GL_BACK);
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    time0 = glutGet(GLUT_ELAPSED_TIME);
    model_init();
#if FFMPEG
    ffmpeg_encoder_start("tmp.mpg", AV_CODEC_ID_MPEG1VIDEO, 25, width, height);
#endif
}

static void deinit(void)  {
    printf("FPS = %f\n", 1000.0 * nframes / (double)(glutGet(GLUT_ELAPSED_TIME) - time0));
    free(pixels);
#if LIBPNG
    if (output_formats & LIBPNG_BIT) {
        free(png_bytes);
        free(png_rows);
    }
#endif
#if FFMPEG
    if (output_formats & FFMPEG_BIT) {
        ffmpeg_encoder_finish();
        free(rgb);
    }
#endif
    if (offscreen) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo_color);
        glDeleteRenderbuffers(1, &rbo_depth);
    }
}

static void draw_scene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(angle, 0.0f, 0.0f, -1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f( 0.0f,  0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 0.5f, -0.5f, 0.0f);
    glEnd();
}

static void display(void) {
    char filename[SCREENSHOT_MAX_FILENAME];
    draw_scene();
    if (offscreen) {
        glFlush();
    } else {
        glutSwapBuffers();
    }

	/* BILL : Here is where the render_frame code was. */

    nframes++;
    if (model_finished())
        exit(EXIT_SUCCESS);
}

static void idle(void) {
    while (model_update());
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    int arg;
    GLint glut_display;

    /* CLI args. */
    glutInit(&argc, argv);
    arg = 1;
    if (argc > arg) {
        offscreen = (argv[arg][0] == '1');
    } else {
        offscreen = 1;
    }
    arg++;
    if (argc > arg) {
        max_nframes = strtoumax(argv[arg], NULL, 10);
    }
    arg++;
    if (argc > arg) {
        width = strtoumax(argv[arg], NULL, 10);
    }
    arg++;
    if (argc > arg) {
        height = strtoumax(argv[arg], NULL, 10);
    }
    arg++;
    if (argc > arg) {
        output_formats = strtoumax(argv[arg], NULL, 10);
    }

    /* Work. */
    if (offscreen) {
        /* TODO: if we use anything smaller than the window, it only renders a smaller version of things. */
        /*glutInitWindowSize(50, 50);*/
        glutInitWindowSize(width, height);
        glut_display = GLUT_SINGLE;
    } else {
        glutInitWindowSize(width, height);
        glutInitWindowPosition(100, 100);
        glut_display = GLUT_DOUBLE;
    }
    glutInitDisplayMode(glut_display | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
    if (offscreen) {
        /* TODO: if we hide the window the program blocks. */
        /*glutHideWindow();*/
    }
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    atexit(deinit);
    glutMainLoop();
    return EXIT_SUCCESS;
}