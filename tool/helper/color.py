import random
from math import sin, cos


def rgb2lab(r, g, b):
  r = r / 255
  g = g / 255
  b = b / 255

  r = ((r + 0.055) / 1.055)**2.4 if r > 0.04045 else r / 12.92
  g = ((g + 0.055) / 1.055)**2.4 if g > 0.04045 else g / 12.92
  b = ((b + 0.055) / 1.055)**2.4 if b > 0.04045 else b / 12.92

  x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375
  y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750
  z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041

  x /= 0.950456
  y /= 1.0
  z /= 1.088754

  x = x**(1 / 3) if x > 0.008856 else (903.3 * x + 16) / 116
  y = y**(1 / 3) if y > 0.008856 else (903.3 * y + 16) / 116
  z = z**(1 / 3) if z > 0.008856 else (903.3 * z + 16) / 116

  l = y * 100
  a = (x - y) * 127
  b = (y - z) * 127

  return l, a, b


def lab2rgb(l, a, b):
  y = l / 100
  x = a / 127 + y
  z = y - b / 127

  x = x**3 if x > 0.206893 else ((x * 116) - 16) / 903.3
  y = y**3 if y > 0.206893 else ((y * 116) - 16) / 903.3
  z = z**3 if z > 0.206893 else ((z * 116) - 16) / 903.3

  x *= 0.950456
  y *= 1.0
  z *= 1.088754

  r = x * 3.2404542 + y * -1.5371385 + z * -0.4985314
  g = x * -0.9692660 + y * 1.8760108 + z * 0.0415560
  b = x * 0.0556434 + y * -0.2040259 + z * 1.0572252

  r = 1.055 * r**(1 / 2.4) - 0.055 if r > 0.0031308 else 12.92 * r
  g = 1.055 * g**(1 / 2.4) - 0.055 if g > 0.0031308 else 12.92 * g
  b = 1.055 * b**(1 / 2.4) - 0.055 if b > 0.0031308 else 12.92 * b

  return int(r * 255), int(g * 255), int(b * 255)


def randomize_color(r, g, b, l=8):
  x, y, z = rgb2lab(r, g, b)
  a = random.randint(0, 359) / 180 * 3.1415926535
  l = random.randint(0, 4 * l) / 4
  y += sin(a) * l
  z += cos(a) * l
  r, g, b = lab2rgb(x, y, z)
  return min(max(r, 0), 255), min(max(g, 0), 255), min(max(b, 0), 255)
