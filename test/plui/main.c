
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/keysym.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NO_STD 0
#include <define.h>
#include <pl2d/fb.h>
#include <plds/server/api.h>
#include <sys.h>

u32 screen_width;
u32 screen_height;

Display *display;
int      screen;
Window   window;

Atom wmDeleteMessage;

XShmSegmentInfo shm_info;
XImage         *image;

bool exit_flag = false;

void program_exit() {
  exit_flag = true;
}

void create_img(u32 width, u32 height) {
  image            = XShmCreateImage(display, NULL, 24, ZPixmap, 0, &shm_info, width, height);
  shm_info.shmid   = shmalloc((size_t)width * height * 4);
  shm_info.shmaddr = image->data = shmref(shm_info.shmid, null);
  shm_info.readOnly              = false;
  XShmAttach(display, &shm_info);
}

void destroy_img() {
  XShmDetach(display, &shm_info);
  shmunref(shm_info.shmaddr);
  XDestroyImage(image);
}

void recreate_img(u32 width, u32 height) {
  if (image) destroy_img();
  create_img(width, height);
}

static void init_xlib(u32 width, u32 height) {
  screen_width = width, screen_height = height;

  display     = XOpenDisplay(NULL);
  screen      = DefaultScreen(display);
  Window root = RootWindow(display, screen);
  window = XCreateSimpleWindow(display, root, 0, 0, width, height, 1, BlackPixel(display, screen),
                               WhitePixel(display, screen));
  XSelectInput(display, window,
               ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask |
                   ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask |
                   StructureNotifyMask);
  XMapWindow(display, window);
  XFlush(display);

  XSynchronize(display, True);

  wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", false);

  { // 隐藏鼠标指针
    XColor  black   = {};
    uint8_t data[8] = {};
    Pixmap  bitmap  = XCreateBitmapFromData(display, root, (const char *)data, 1, 1);
    Cursor  cursor  = XCreatePixmapCursor(display, bitmap, bitmap, &black, &black, 0, 0);
    XDefineCursor(display, window, cursor);
    XFreeCursor(display, cursor);
    XFreePixmap(display, bitmap);
  }

  create_img(width, height);
}

void screen_flush() {
  XShmPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, screen_width,
               screen_height, true);
  XFlush(display);

  static u64 start_time = 0;
  if (start_time == 0) start_time = monotonic_us();
  static u64 nframe = 0;
  nframe++;
  u64 next_time = start_time + 1000000 * nframe / 60;
  while (true) {
    u64 time = monotonic_us();
    if (next_time <= time) break;
    usleep(next_time - time);
  }

  static u64 old_time = 0;
  if (old_time == 0) {
    old_time = monotonic_us();
  } else {
    u64 time = monotonic_us();
    printf("%lf\n", 1e6 / (time - old_time));
    old_time = time;
  }
}

int loop_body(XEvent e, int pending) {
  if (e.type == ClientMessage) {
    if (e.xclient.message_type == wmDeleteMessage) goto quit;
  }

  if (e.type == Expose) {}

  if (e.type == ButtonPress) {
    if (--e.xbutton.button < 3) plds_on_button_down(e.xbutton.button, e.xbutton.x, e.xbutton.y);
    if (e.xbutton.button == 3) plds_on_scroll(-3);
    if (e.xbutton.button == 4) plds_on_scroll(3);
  }

  if (e.type == ButtonRelease) {
    if (--e.xbutton.button < 3) plds_on_button_up(e.xbutton.button, e.xbutton.x, e.xbutton.y);
  }

  if (e.type == MotionNotify) { plds_on_mouse_move(e.xmotion.x, e.xmotion.y); }

  if (e.type == KeyPress) {
    KeySym key;
    XLookupString(&e.xkey, NULL, 0, &key, NULL);
    plds_on_key_down(key);
  }

  if (e.type == KeyRelease) {
    KeySym key;
    XLookupString(&e.xkey, NULL, 0, &key, NULL);
    plds_on_key_up(key);
  }

  if (e.type == ConfigureNotify) {
    if (e.xconfigure.width != screen_width || e.xconfigure.height != screen_height) {
      screen_width  = e.xconfigure.width;
      screen_height = e.xconfigure.height;
      recreate_img(screen_width, screen_height);
      plds_on_screen_resize(image->data, screen_width, screen_height, pl2d_PixFmt_BGRA);
    }
  }

  // 如果是当前帧处理的最后一个事件，就发送 flush
  if (pending == 0) plds_flush();

  return 0;

quit:
  exit_flag = true;
  return 0;
}

int main() {
  init_xlib(1280, 720);

  {
    u64 time = monotonic_us();
    int ret  = plds_init(image->data, screen_width, screen_height, pl2d_PixFmt_BGRA);
    if (ret < 0) return -ret;
    printf("%ld us\n", monotonic_us() - time);
  }

  int    ret;
  XEvent event;
  while (!exit_flag) {
    XNextEvent(display, &event);

    ret = loop_body(event, XPending(display));
    if (ret) break;
  }

  destroy_img();

  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return ret;
}
