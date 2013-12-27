#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xtest.h>
#include "helpers.h"
#include "xdo.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
        err("No arguments given.\n");

    void (*action) (xcb_window_t win);

    if (strcmp(argv[1], "id") == 0)
        action = window_id;
    else if (strcmp(argv[1], "pid") == 0)
        action = window_pid;
    else if (strcmp(argv[1], "activate") == 0)
        action = window_activate;
    else if (strcmp(argv[1], "hide") == 0)
        action = window_hide;
    else if (strcmp(argv[1], "show") == 0)
        action = window_show;
    else if (strcmp(argv[1], "move") == 0)
        action = window_move;
    else if (strcmp(argv[1], "resize") == 0)
        action = window_resize;
    else if (strcmp(argv[1], "close") == 0)
        action = window_close;
    else if (strcmp(argv[1], "kill") == 0)
        action = window_kill;
    else if (strcmp(argv[1], "raise") == 0)
        action = window_raise;
    else if (strcmp(argv[1], "lower") == 0)
        action = window_lower;
    else if (strcmp(argv[1], "key") == 0)
        action = key_press_release;
    else if (strcmp(argv[1], "button") == 0)
        action = button_press_release;
    else if (strcmp(argv[1], "-h") == 0)
        return usage();
    else if (strcmp(argv[1], "-v") == 0)
        return version();
    else
        err("Unknown action: '%s'.\n", argv[1]);

    init();
    argc--, argv++;
    char opt;
    while ((opt = getopt(argc, argv, "rcCdDn:N:p:k:sx:y:h:w:")) != -1) {
        switch (opt) {
            case 'r':
                cfg.wid = VALUE_DIFFERENT;
                break;
            case 'c':
                cfg.class = VALUE_SAME;
                break;
            case 'C':
                cfg.class = VALUE_DIFFERENT;
                break;
            case 'd':
                cfg.desktop = VALUE_SAME;
                break;
            case 'D':
                cfg.desktop = VALUE_DIFFERENT;
                break;
            case 'n':
                cfg.instance_name = optarg;
                break;
            case 'N':
                cfg.class_name = optarg;
                break;
            case 'p':
                cfg.pid = atoi(optarg);
                break;
            case 'k':
                cfg.evt_code = atoi(optarg);
                break;
            case 's':
                cfg.symb_desks = true;
                break;
            case 'x':
                cfg.x = optarg;
                break;
            case 'y':
                cfg.y = optarg;
                break;
            case 'w':
                cfg.width = optarg;
                break;
            case 'h':
                cfg.height = optarg;
                break;
        }
    }

    int num = argc - optind;
    char **args = argv + optind;

    setup();

    int hits = 0;
    xcb_window_t win = XCB_NONE;
    char class[MAXLEN] = {0};
    uint32_t desktop = 0;
    if (cfg.wid != VALUE_IGNORE || cfg.class != VALUE_IGNORE)
        get_active_window(&win);
    if (cfg.class != VALUE_IGNORE)
        get_class(win, class, sizeof(class));
    if (cfg.desktop != VALUE_IGNORE)
        get_current_desktop(&desktop);

    if (num > 0) {
        char *end;
        for (int i = 0; i < num; i++) {
            errno = 0;
            long int w = strtol(args[i], &end, 0);
            if (errno != 0 || *end != '\0') {
                warn("Invalid window ID: '%s'.\n", args[i]);
            } else if (match(w, win, desktop, class)) {
                (*action)(w);
                hits++;
            }
        }
    } else {
        if (cfg.class == VALUE_IGNORE
                && cfg.desktop == VALUE_IGNORE
                && cfg.class_name == NULL
                && cfg.instance_name == NULL
                && cfg.pid == 0) {
            xcb_window_t win;
            get_active_window(&win);
            if (win != XCB_NONE) {
                (*action)(win);
                hits++;
            }
        } else {
            xcb_query_tree_reply_t *qtr = xcb_query_tree_reply(dpy, xcb_query_tree(dpy, root), NULL);
            if (qtr == NULL)
                err("Failed to query the window tree.\n");
            int len = xcb_query_tree_children_length(qtr);
            xcb_window_t *wins = xcb_query_tree_children(qtr);
            for (int i = 0; i < len; i++) {
                xcb_window_t w = wins[i];
                if (match(w, win, desktop, class)) {
                    (*action)(w);
                    hits++;
                }
            }
            free(qtr);
        }
    }

    finish();
    if (hits > 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

bool match(xcb_window_t w, xcb_window_t win, uint32_t desktop, char* class)
{
    char c[MAXLEN] = {0}, i[MAXLEN] = {0};
    uint32_t d, p;
    return (cfg.wid == VALUE_IGNORE || (cfg.wid == VALUE_DIFFERENT && w != win)) &&
        (cfg.class == VALUE_IGNORE ||
         (get_class(w, c, sizeof(c)) &&
          ((cfg.class == VALUE_SAME && strcmp(class, c) == 0) ||
           (cfg.class == VALUE_DIFFERENT && strcmp(class, c) != 0)))) &&
        (cfg.class_name == NULL || (get_class(w, c, sizeof(c))
                                    && strcmp(cfg.class_name, c) == 0)) &&
        (cfg.instance_name == NULL || (get_instance(w, i, sizeof(i))
                                       && strcmp(cfg.instance_name, i) == 0)) &&
        (cfg.pid == 0 || (get_pid(w, &p) && p == cfg.pid)) &&
        (cfg.desktop == VALUE_IGNORE ||
         (get_desktop(w, &d) &&
          ((cfg.desktop == VALUE_SAME && DESKEQ(desktop, d)) ||
           (cfg.desktop == VALUE_DIFFERENT && !DESKEQ(desktop, d)))));
}

void init(void)
{
    cfg.class = cfg.desktop = cfg.wid = VALUE_IGNORE;
    cfg.class_name = cfg.instance_name = NULL;
    cfg.x = cfg.y = cfg.width = cfg.height = NULL;
    cfg.pid = 0;
    cfg.evt_code = XCB_NONE;
    cfg.symb_desks = false;
}

int usage(void)
{
    printf("xdo ACTION [OPTIONS] [WID ...]\n");
    return EXIT_SUCCESS;
}

int version(void)
{
    printf("%s\n", VERSION);
    return EXIT_SUCCESS;
}

void setup(void)
{
    dpy = xcb_connect(NULL, &default_screen);
    if (xcb_connection_has_error(dpy))
        err("Can't open display.\n");
    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
    if (screen == NULL)
        err("Can't acquire screen.\n");
    root = screen->root;
    ewmh = malloc(sizeof(xcb_ewmh_connection_t));
    if (xcb_ewmh_init_atoms_replies(ewmh, xcb_ewmh_init_atoms(dpy, ewmh), NULL) == 0)
        err("Can't initialize EWMH atoms.\n");
}

void finish(void)
{
    xcb_flush(dpy);
    xcb_ewmh_connection_wipe(ewmh);
    free(ewmh);
    xcb_disconnect(dpy);
}

void get_atom(char *name, xcb_atom_t *atom)
{
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(dpy, xcb_intern_atom(dpy, 0, strlen(name), name), NULL);
    if (reply != NULL)
        *atom = reply->atom;
    else
        err("Could not allocate atom: '%s'.\n", name);
    free(reply);
}

void get_active_window(xcb_window_t *win)
{
    if (xcb_ewmh_get_active_window_reply(ewmh, xcb_ewmh_get_active_window(ewmh, default_screen), win, NULL) != 1)
        err("Can't determine the active window.\n");
}

bool get_class(xcb_window_t win, char *class, size_t len)
{
    xcb_icccm_get_wm_class_reply_t icr;
    if (xcb_icccm_get_wm_class_reply(dpy, xcb_icccm_get_wm_class(dpy, win), &icr, NULL) == 1) {
        strncpy(class, icr.class_name, len);
        return true;
    } else {
        return false;
    }
}

bool get_instance(xcb_window_t win, char *instance, size_t len)
{
    xcb_icccm_get_wm_class_reply_t icr;
    if (xcb_icccm_get_wm_class_reply(dpy, xcb_icccm_get_wm_class(dpy, win), &icr, NULL) == 1) {
        strncpy(instance, icr.instance_name, len);
        return true;
    } else {
        return false;
    }
}

bool get_pid(xcb_window_t win, uint32_t *pid)
{
    return (xcb_ewmh_get_wm_pid_reply(ewmh, xcb_ewmh_get_wm_pid(ewmh, win), pid, NULL) == 1);
}

bool get_desktop(xcb_window_t win, uint32_t *desktop)
{
    return (xcb_ewmh_get_wm_desktop_reply(ewmh, xcb_ewmh_get_wm_desktop(ewmh, win), desktop, NULL) == 1 && (*desktop != ALL_DESKS || cfg.symb_desks));
}

bool get_current_desktop(uint32_t *desktop)
{
    return (xcb_ewmh_get_current_desktop_reply(ewmh, xcb_ewmh_get_current_desktop(ewmh, default_screen), desktop, NULL) == 1);
}

void window_close(xcb_window_t win)
{
    xcb_client_message_event_t e;
    xcb_atom_t atom;
    get_atom("WM_DELETE_WINDOW", &atom);

    e.response_type = XCB_CLIENT_MESSAGE;
    e.window = win;
    e.format = 32;
    e.sequence = 0;
    e.type = ewmh->WM_PROTOCOLS;
    e.data.data32[0] = atom;
    e.data.data32[1] = XCB_CURRENT_TIME;

    xcb_send_event(dpy, false, win, XCB_EVENT_MASK_NO_EVENT, (char *) &e);
}

void window_kill(xcb_window_t win)
{
    xcb_kill_client(dpy, win);
}

void window_hide(xcb_window_t win)
{
    xcb_unmap_window(dpy, win);
}

void window_show(xcb_window_t win)
{
    xcb_map_window(dpy, win);
}

void window_raise(xcb_window_t win)
{
    uint32_t values[] = {XCB_STACK_MODE_ABOVE};
    xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

void window_lower(xcb_window_t win)
{
    uint32_t values[] = {XCB_STACK_MODE_BELOW};
    xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

#define HANDLE_COORD(v) \
    if (cfg.v != NULL) { \
        uint32_t v = atoi(cfg.v); \
        if (ISRELA(cfg.v)) \
            values[i++] += v; \
        else \
            values[i++] = v; \
    }

void window_move(xcb_window_t win)
{
    xcb_get_geometry_reply_t *geo = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
    if (geo == NULL)
        return;
    uint32_t values[2] = {geo->x, geo->y};
    int i = 0;
    HANDLE_COORD(x)
    HANDLE_COORD(y)
    if (i == 0)
        return;
    xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
}

void window_resize(xcb_window_t win)
{
    xcb_get_geometry_reply_t *geo = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
    if (geo == NULL)
        return;
    uint32_t values[2] = {geo->width, geo->height};
    int i = 0;
    HANDLE_COORD(width)
    HANDLE_COORD(height)
    if (i == 0)
        return;
    xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

#undef HANDLE_COORD

void window_activate(xcb_window_t win)
{
    xcb_client_message_event_t e;
    e.response_type = XCB_CLIENT_MESSAGE;
    e.window = win;
    e.format = 32;
    e.sequence = 0;
    e.type = ewmh->_NET_ACTIVE_WINDOW;
    e.data.data32[0] = 1;
    e.data.data32[1] = XCB_CURRENT_TIME;

    xcb_send_event(dpy, false, root, XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (char *) &e);
}

void window_id(xcb_window_t win)
{
    printf("0x%X\n", win);
}

void window_pid(xcb_window_t win)
{
    uint32_t pid;
    if (get_pid(win, &pid))
        printf("%i\n", pid);
}

void fake_input(xcb_window_t win, uint8_t evt, uint8_t code)
{
    xcb_test_fake_input(dpy, evt, code, XCB_CURRENT_TIME, win, 0, 0, 0);
}

void key_press_release(xcb_window_t win)
{
    fake_input(win, XCB_KEY_PRESS, cfg.evt_code);
    fake_input(win, XCB_KEY_RELEASE, cfg.evt_code);
}

void button_press_release(xcb_window_t win)
{
    fake_input(win, XCB_BUTTON_PRESS, cfg.evt_code);
    fake_input(win, XCB_BUTTON_RELEASE, cfg.evt_code);
}
