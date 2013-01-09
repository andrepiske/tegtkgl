#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "tegtkgl.h"
#include <GL/gl.h>
#include <math.h>

static gboolean do_the_gl = TRUE;

static
void destroy_the_gl(gpointer ud) {
    do_the_gl = FALSE;
}

static
gboolean draw_the_gl(gpointer ud) {
    static float s = 0.f;
    GtkWidget *gl = (GtkWidget*)ud;

    if (!do_the_gl)
        return FALSE;

    // this is very important, as OpenGL has a somewhat global state. 
    // this will set the OpenGL state to this very widget.
    te_gtkgl_make_current(TE_GTKGL(gl));

    // some triangle rotation stuff
    s += 0.03f;

    // more bureaucracy
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glViewport(0, 0, 200, 200);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glRotatef(cos(s)*360.f-180.f, 0.f, 0.f, 1.0);
    glColor4ub(0xed, 0xb9, 0x1e, 0xff);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.7f, -0.5f);
    glVertex2f(0.4f, -0.5f);
    glVertex2f(0.f, 0.8f);
    glEnd();

    // this is also very important
    te_gtkgl_swap(TE_GTKGL(gl));

    return TRUE;
}

int main(int argc, char *argv[]) {

    GtkWidget *win, *cnt, *gl;
    int i;

    // initialize GTK+
    gtk_init(&argc, &argv);

    // create the window and set it to quit application when closed
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(gtk_main_quit), 0);

    // create the OpenGL widget
    gl = te_gtkgl_new();

    // set a callback that will stop the timer from drawing
    g_signal_connect(G_OBJECT(gl), "destroy", G_CALLBACK(destroy_the_gl), 0);


    // bureaucracy and show things on screen
    cnt = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(cnt), gl, 0, 0, 1, 1);
    gtk_container_add(GTK_CONTAINER(win), cnt);
    gtk_widget_set_size_request(gl, 200, 200);
    gtk_widget_show_all(win);

    // drawing timer
    g_timeout_add(10, draw_the_gl, (gpointer)gl);
    gtk_main();

    return 0;
}

