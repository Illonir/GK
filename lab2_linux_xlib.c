
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

int red, green, blue, yellow;
unsigned long foreground, background;

XPoint points[10];
int pnum = 10;
XPoint gap[3];
int gnum = 3;
int moved = -1;

XPoint toPoint(int x, int y) {
  XPoint p;
  p.x = x;
  p.y = y;
  return p;
}





//*************************************************************************************************************************
//funkcja przydzielania kolorow

int AllocNamedColor(char *name, Display* display, Colormap colormap)
  {
    XColor col;
    XParseColor(display, colormap, name, &col);
    XAllocColor(display, colormap, &col);
    return col.pixel;
  } 

//*************************************************************************************************************************
// inicjalizacja zmiennych globalnych okreslajacych kolory

int init_colors(Display* display, int screen_no, Colormap colormap)
{
  background = WhitePixel(display, screen_no);  //niech tlo bedzie biale
  foreground = BlackPixel(display, screen_no);  //niech ekran bedzie czarny
  red=AllocNamedColor("red", display, colormap);
  green=AllocNamedColor("green", display, colormap);
  blue=AllocNamedColor("blue", display, colormap);
  yellow=AllocNamedColor("yellow", display, colormap);
}

//*************************************************************************************************************************
// Glowna funkcja zawierajaca petle obslugujaca zdarzenia */

int main(int argc, char *argv[])
{
  char            icon_name[] = "Grafika";
  char            title[]     = "Grafika komputerowa";
  Display*        display;    //gdzie bedziemy wysylac dane (do jakiego X servera)
  Window          window;     //nasze okno, gdzie bedziemy dokonywac roznych operacji
  GC              gc;         //tu znajduja sie informacje o parametrach graficznych
  XEvent          event;      //gdzie bedziemy zapisywac pojawiajace sie zdarzenia
  KeySym          key;        //informacja o stanie klawiatury 
  Colormap        colormap;
  int             screen_no;
  XSizeHints      info;       //informacje typu rozmiar i polozenie ok
  
  char            buffer[8];  //gdzie bedziemy zapamietywac znaki z klawiatury
  int             hm_keys;    //licznik klawiszy
  int             to_end;

  display    = XOpenDisplay("");                //otworz polaczenie z X serverem pobierz dane od zmiennej srodowiskowej DISPLAY ("")
  screen_no  = DefaultScreen(display);          //pobierz domyslny ekran dla tego wyswietlacza (0)
  colormap = XDefaultColormap(display, screen_no);
  init_colors(display, screen_no, colormap);

  //okresl rozmiar i polozenie okna
  info.x = 100;
  info.y = 150;
  info.width = 500;
  info.height = 300;
  info.flags = PPosition | PSize;

  //majac wyswietlacz, stworz okno - domyslny uchwyt okna
  window = XCreateSimpleWindow(display, DefaultRootWindow(display),info.x, info.y, info.width, info.height, 7/* grubosc ramki */, foreground, background);
  XSetStandardProperties(display, window, title, icon_name, None, argv, argc, &info);
  //utworz kontekst graficzny do zarzadzania parametrami graficznymi (0,0) domyslne wartosci
  gc = XCreateGC(display, window, 0, 0);
  XSetBackground(display, gc, background);
  XSetForeground(display, gc, foreground);

  //okresl zdarzenia jakie nas interesuja, np. nacisniecie klawisza
  XSelectInput(display, window, (KeyPressMask | ExposureMask | ButtonPressMask| ButtonReleaseMask | Button1MotionMask));
  XMapRaised(display, window);  //wyswietl nasze okno na samym wierzchu wszystkich okien
      
  to_end = FALSE;

  points[0] = toPoint(100, 100);
  points[1] = toPoint(200, 100);
  points[2] = toPoint(300, 200);
  points[3] = toPoint(300, 300);
  points[4] = toPoint(200, 350);
  points[5] = toPoint(300, 500);
  points[6] = toPoint(250, 500);
  points[7] = toPoint(150, 350);
  points[8] = toPoint(150, 500);
  points[9] = toPoint(100, 500);

  gap[0] = toPoint(150, 200);
  gap[1] = toPoint(250, 250);
  gap[2] = toPoint(150, 300);

 /* petla najpierw sprawdza, czy warunek jest spelniony
     i jesli tak, to nastepuje przetwarzanie petli
     a jesli nie, to wyjscie z petli, bez jej przetwarzania */
  while (to_end == FALSE)
  {
    XNextEvent(display, &event);  // czekaj na zdarzenia okreslone wczesniej przez funkcje XSelectInput

    switch(event.type)
    {
      case Expose:
        if (event.xexpose.count == 0)
        {
          XClearWindow(display, window);
          //XDrawLines(
          XFillPolygon(
            event.xexpose.display,
            event.xexpose.window,
            gc,
            points,
            10,
            Complex,
            CoordModeOrigin
          );
          XSetForeground(display, gc, background);
          XFillPolygon(
            event.xexpose.display,
            event.xexpose.window,
            gc,
            gap,
            3,
            Complex,
            CoordModeOrigin
          );
          XSetForeground(display, gc, foreground);
        }
        break;

      case MappingNotify:
        XRefreshKeyboardMapping(&event.xmapping); // zmiana ukladu klawiatury - w celu zabezpieczenia sie przed taka zmiana trzeba to wykonac
        break;

      case ButtonPress:
        if (event.xbutton.button == Button1)  // sprawdzenie czy wcisnieto lewy przycisk		
        {
          int i = 0;
          int j = 0;
          int rad = 25;
          int mx;
          int my;
          Window bw;
          Window bw2;
          int j1 = 0;
          unsigned int mask = 0;
          int rpoint = 0;
          if (XQueryPointer(display, window, &bw, &bw2, &j1, &j, &mx, &my, &mask) == TRUE)
          {
            printf("Mouse at %d %d\n", mx, my);
            if (moved == -1)
            {
              for (i = 0; i < pnum; i++)
              {
                //printf(">%d", i);
                if ((mx > (points[i].x - rad) && mx < (points[i].x + rad)) &&
                    (my > (points[i].y - rad) && my < (points[i].y + rad)))
                    {
                      printf("\tMouse hit point %d %d\n", points[i].x, points[i].y);
                      moved = i;
                      rpoint = 1;
                      break;
                    }
              }
              if (rpoint == 0)
              {
                for (i = 0; i < gnum; i++)
                {
                  if ((mx > (gap[i].x - rad) && mx < (gap[i].x + rad)) &&
                      (my > (gap[i].y - rad) && my < (gap[i].y + rad)))
                    {
                      printf("\tMouse hit gap point %d %d\n", gap[i].x, gap[i].y);
                      moved = pnum + i;
                      break;
                    }
                }
              }
            }
            else {
              printf("\tMoving pt %d\n", moved);
              if (moved >= pnum) {
                gap[moved - pnum].x = mx;
                gap[moved - pnum].y = my;
              } else {
                points[moved].x = mx;
                points[moved].y = my;
              }
              moved = -1;
              event.type = Expose;	  
              event.xexpose.count = 0;
              XSendEvent(display, window, 0, ExposureMask, &event);  // wymuszenie komunikatu z zadaniem odswiezenia okna
            }
          }
   		              
    
        }
        break;


      case KeyPress:
        hm_keys = XLookupString(&event.xkey, buffer, 8, &key, 0);
        if (hm_keys == 1)
        {
          if (buffer[0] == 'q') to_end = TRUE;        // koniec programu
          
        }

      default:
        break;
    }
  }

  XFreeGC(display, gc);
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}
