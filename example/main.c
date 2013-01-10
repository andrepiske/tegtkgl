#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "tegtkgl.h"
#include <GL/gl.h>
#include <math.h>

static gboolean do_the_gl = TRUE;
static GtkWidget *g_gl_wid = 0;

static
void destroy_the_gl(GtkWidget *wid, gpointer ud) {
    do_the_gl = FALSE;
}

static
gboolean on_clicked(GtkWidget *wid, GdkEvent *ev, gpointer user_data) {
    printf("clicked at %.3fx%.3f with button %d\n",
        ev->button.x, ev->button.y, ev->button.button);
    return TRUE;
}

static
gboolean draw_the_gl(gpointer ud) {
    static float s = 0.f;
    GtkWidget *gl = g_gl_wid;
    GdkWindow *wnd;

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

static void
click_the_button(GtkWidget *bt, gpointer ud) {
    GtkWidget *grid, *gl;
    int n;
    grid = *(GtkWidget**)ud;

    for (n = 0; n < 500; ++n) {
        gl = gtk_grid_get_child_at(GTK_GRID(grid), 0, 1);

        // do_the_gl = FALSE;
        // g_object_unref(G_OBJECT(gl));
        do_the_gl = 0;
        gtk_container_remove(GTK_CONTAINER(grid), gl);

        gl = te_gtkgl_new();
        gtk_widget_set_size_request(gl, 200, 200);
        gtk_grid_attach(GTK_GRID(grid), gl, 0, 1, 1, 1);

        g_gl_wid = gl;
        do_the_gl = 1;

        gtk_widget_show_all(grid);
        gdk_window_process_all_updates();
    }

    // gl = te_gtkgl_new();
    // gl = gtk_grid_get_child_at(GTK_GRID(grid), 0, 1);
}

int main(int argc, char *argv[]) {

    GtkWidget *win, *cnt, *gl, *bt1;
    int i;

    // initialize GTK+
    gtk_init(&argc, &argv);

    // create the window and set it to quit application when closed
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(gtk_main_quit), 0);

    // create the OpenGL widget
    g_gl_wid = gl = te_gtkgl_new();

    bt1 = gtk_button_new_with_label("one");
    g_signal_connect(G_OBJECT(bt1), "clicked", G_CALLBACK(click_the_button), (gpointer)&cnt);

    // set a callback that will stop the timer from drawing
    g_signal_connect(G_OBJECT(gl), "destroy", G_CALLBACK(destroy_the_gl), 0);

    gtk_widget_add_events(gl,  GDK_ALL_EVENTS_MASK);
    g_signal_connect(G_OBJECT(gl), "button-press-event", G_CALLBACK(on_clicked), 0);

    // our layout
    cnt = gtk_grid_new();

    // create menu
    {
        int i;
        GtkWidget *mb, *main, *subm;
        mb = gtk_menu_bar_new();
        main = gtk_menu_item_new_with_label("File");

        gtk_container_add(GTK_CONTAINER(mb), main);
        gtk_grid_attach(GTK_GRID(cnt), mb, 0, 0, 2, 1);

        subm = gtk_menu_new();
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(main), subm);
        for (i =0; i < 16; ++i) {
            char n[64];
            GtkWidget *k;
            sprintf(n, "Item %02d", i+1);
            k = gtk_menu_item_new_with_label(n);
            gtk_container_add(GTK_CONTAINER(subm), k);
        }
    }

    // bureaucracy and show things on screen
    gtk_grid_attach(GTK_GRID(cnt), gl, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(cnt), bt1, 1, 1, 1, 1);
    gtk_container_add(GTK_CONTAINER(win), cnt);
    gtk_widget_set_size_request(gl, 200, 200);
    gtk_widget_show_all(win);

    g_timeout_add_full(1000, 10, draw_the_gl, 0, 0);

    gtk_main();
    return 0;
}

