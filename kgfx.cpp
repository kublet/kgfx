#include <TFT_eSPI.h>

#include "kgfx.h"

/***************************************************************************************
** Function name:           init
** Description:             Initializes GFX library
***************************************************************************************/
void KGFX::init() {
  tf.begin();
  palette[0] = TFT_BLACK;
}

/***************************************************************************************
** Function name:           clear
** Description:             Paints screen black
***************************************************************************************/
void KGFX::clear() {
  t.fillScreen(TFT_BLACK);
  tf.fillScreen(TFT_BLACK);  
}

/***************************************************************************************
** Function name:           tft
** Description:             Returns tft instance
***************************************************************************************/
 TFT_eSPI_ext KGFX::tft() {
  return tf;
}

/***************************************************************************************
** Function name:           createSprite
** Description:             Creates sprite with given width and height
***************************************************************************************/
TFT_eSprite KGFX::createSprite(int width, int height) {
  TFT_eSprite spr = TFT_eSprite(&tf);
  spr.setColorDepth(16);
  spr.createSprite(width, height);
  return spr;
}

/***************************************************************************************
** Function name:           drawText
** Description:             Draws text to given sprite
***************************************************************************************/
void KGFX::drawText(TFT_eSprite &spr, const char *txt, const tftfont_t &f, int color, int x, int y) {
  tf.TTFdestination(&spr);
  spr.fillSprite(TFT_BLACK);

  tf.setTTFFont(f);
  tf.setTextColor(color, TFT_BLACK);
  tf.setCursor(0,0);
  tf.print(txt);

  spr.pushSprite(x, y);
}

/***************************************************************************************
** Function name:           drawText
** Description:             Draws text to screen
***************************************************************************************/
void KGFX::drawText(const char *txt, const tftfont_t &f, int color, int x, int y) {
  tf.setTTFFont(f);
  tf.setTextColor(color, TFT_BLACK);
  tf.setCursor(x,y);
  tf.print(txt);
}

/***************************************************************************************
** Function name:           createChartSprite
** Description:             Creates chart sprite
***************************************************************************************/
void KGFX::createChartSprite() {
  chartSpr.setColorDepth(16);
  chartSpr.createSprite(240,80);
}

/***************************************************************************************
** Function name:           deleteSprite
** Description:             Deletes given sprite
***************************************************************************************/
void KGFX::deleteSprite(TFT_eSprite &spr) {
  spr.deleteSprite();
}

/***************************************************************************************
** Function name:           deleteChartSprite
** Description:             Deletes chart sprite
***************************************************************************************/
void KGFX::deleteChartSprite() {
  chartSpr.deleteSprite();
}

/***************************************************************************************
** Function name:           drawChart
** Description:             Draws chart to sprite
***************************************************************************************/
void KGFX::drawChart(std::vector<float> arr, int color, int y) {
  tf.TTFdestination(&chartSpr);
  chartSpr.fillSprite(TFT_BLACK);

  createPalette(color);
  chartSpr.createPalette(palette);

  int *fa = fmtChartArray(arr);
  for (int i=0;i<(chartLen-2);i++) {
    drawGraphLine(i*7, fa[i+1], (i+1)*7, fa[i+2], color);
  }

  for (int i=0;i<210;i++) {
    for (int j=0;j<80;j++) {
      if (TFT_BLACK == chartSpr.readPixel(i, j) && (j>0 && TFT_BLACK != chartSpr.readPixel(i, j-1))) {
        drawVGradient(i, j, 5);
        break;
      }
    }
  }

  chartSpr.pushSprite(0, y);
}

void KGFX::createPalette(int color) {
  if (color == K_GREEN) {
    for (int i=0;i<15;i++) {
      palette[i+1] = green_palette[i];
    }
    return;
  }
  if (color == K_RED) {
    for (int i=0;i<15;i++) {
      palette[i+1] = red_palette[i];
    }
    return;
  }
}

void KGFX::drawGraphLine(int x, int y, int x1, int y1, int pcolor) {
  for (int i=0;i<3;i++) {
    chartSpr.drawLine(x, y, x1, y1, pcolor);

    y++; y1++;
  }
}

void KGFX::drawVGradient(int x, int y, int y1) {
  int off = abs(y1 - y);

  for (int i=3;i<=15;i++) {
    for (int j = 0; j < 5; j++) {
      if (off-- && off > 0) {
        continue;
      }

      chartSpr.drawPixel(x, y, palette[i]);
      y++;
    }  
  }
}

int* KGFX::fmtChartArray(std::vector<float> arr) {
  int multi = 1;
  if (arr.size() < 30) {
    Serial.println("Malformed array len: cannot fmt");
    return nullptr;
  }

  float v = arr[0];
  if (v >= 1000) {
  } else {
    if (v >= 10 && v < 1000) {
      multi = 100;
    } else {
      multi = 10000;
    }
  }

  int hi = arr[0] * multi;
  int lo = arr[0] * multi;
  int *fa = new int[chartLen]; 

  for(int i=1;i<chartLen;i++) {
    int v = arr[i] * multi;
    if (v > hi) {
      hi = v;
    }
    if (v < lo) {
      lo = v;
    }
  }

  int diff = hi - lo;
  for(int i=0;i<chartLen;i++) {
    int v = arr[i] * multi;
    fa[i] = 50-round(((v-lo) *50)/diff);
  }

  return fa;
}