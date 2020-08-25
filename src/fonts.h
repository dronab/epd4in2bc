
/* Определяем чтобы предотвратить реверсивное включение */
#ifndef __FONTS_H
#define __FONTS_H

/* Максимальный размер растрового изображения зависит от шрифта font24 (17x24) */
#define MAX_HEIGHT_FONT         24
#define MAX_WIDTH_FONT          17
#define OFFSET_BITMAP           54

#include <stdint.h>

struct sFONT {
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
};

/* Перечисляем шрифты которыми будем пользоваться */

extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;

#endif /* __FONTS_H */
 