
#include <define.h>

static const cstr mime_types[] = {
    "application/java-archive",
    "application/msword",
    "application/pdf",
    "application/vnd.amazon.ebook",
    "application/vnd.ms-excel",
    "application/wasm",
    "application/x-bzip",
    "application/x-bzip2",
    "application/x-cdf",
    "application/x-csh",
    "application/x-rar-compressed",
    "application/zip",
    "audio/aac",
    "audio/mpeg",
    "audio/ogg",
    "audio/wav",
    "audio/webm",
    "audio/x-qoa",
    "image/gif",
    "image/apng",
    "image/jpeg",
    "image/png",
    "image/vnd.microsoft.icon",
    "text/css",
    "text/csv",
    "text/html",
    "video/mp4",
    "video/x-msvideo",
    "video/webm",
    "font/woff",
    "font/woff2",
    "font/ttf",
    "font/otf",
    "image/svg+xml",
    "application/x-7z-compressed",
    "text/x-script",
    "application/x-executable",
    "application/x-msdownload",
    "audio/x-plac",
    "font/x-plff",
};

dlexport cstr filetype(const void *data, size_t size) {
  if (size == 0) return null;
  byte b = ((const byte *)data)[0];
  if (b == 80) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 75) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 3) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 4) return mime_types[11];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 208) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 207) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 17) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 224) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 161) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 177) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 26) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 225) return mime_types[4];
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 37) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 80) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 68) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 70) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 45) return mime_types[2];
          return null;
        }
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 0) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 97) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 115) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 109) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 1) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 0) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 0) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 0) return mime_types[5];
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    if (b == 0) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 1) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 0) return mime_types[22];
        return null;
      }
      if (b == 0) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 24) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 102) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 116) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 121) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 112) {
                  if (size == 8) return null;
                  byte b = ((const byte *)data)[8];
                  if (b == 109) {
                    if (size == 9) return null;
                    byte b = ((const byte *)data)[9];
                    if (b == 112) {
                      if (size == 10) return null;
                      byte b = ((const byte *)data)[10];
                      if (b == 52) {
                        if (size == 11) return null;
                        byte b = ((const byte *)data)[11];
                        if (b == 50) return mime_types[26];
                        return null;
                      }
                      return null;
                    }
                    return null;
                  }
                  return null;
                }
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    if (b == 1) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 0) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 0) return mime_types[31];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 66) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 90) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 104) return mime_types[7];
      return null;
    }
    return null;
  }
  if (b == 67) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 68) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 48) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 48) return mime_types[8];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 35) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 33) {
      byte b = ((const byte *)data)[2];
      if (b == 47) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 98) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 105) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 110) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 47) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 115) {
                  if (size == 8) return null;
                  byte b = ((const byte *)data)[8];
                  if (b == 104) return mime_types[9];
                  return null;
                }
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        if (b == 117) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 115) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 114) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 47) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 98) {
                  if (size == 8) return null;
                  byte b = ((const byte *)data)[8];
                  if (b == 105) {
                    if (size == 9) return null;
                    byte b = ((const byte *)data)[9];
                    if (b == 110) {
                      if (size == 10) return null;
                      byte b = ((const byte *)data)[10];
                      if (b == 47) {
                        if (size == 11) return null;
                        byte b = ((const byte *)data)[11];
                        if (b == 98) {
                          if (size == 12) return null;
                          byte b = ((const byte *)data)[12];
                          if (b == 97) {
                            if (size == 13) return null;
                            byte b = ((const byte *)data)[13];
                            if (b == 115) {
                              if (size == 14) return null;
                              byte b = ((const byte *)data)[14];
                              if (b == 104) return mime_types[9];
                              return null;
                            }
                            return null;
                          }
                          return null;
                        }
                        return null;
                      }
                      return null;
                    }
                    return null;
                  }
                  return null;
                }
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return mime_types[35];
    }
    return null;
  }
  if (b == 82) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 97) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 114) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 33) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 26) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 7) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 0) return mime_types[10];
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    if (b == 73) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 70) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 70) return mime_types[27];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 255) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 241) return mime_types[12];
    if (b == 251) return mime_types[13];
    if (b == 216) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 255) return mime_types[20];
      return null;
    }
    return null;
  }
  if (b == 79) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 103) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 103) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 83) return mime_types[14];
        return null;
      }
      return null;
    }
    if (b == 84) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 84) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 79) return mime_types[32];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 26) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 69) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 223) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 163) return mime_types[28];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 113) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 111) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 97) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 102) return mime_types[17];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 71) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 73) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 70) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 56) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 57) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 97) return mime_types[18];
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 137) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 80) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 78) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 71) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 13) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 10) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 26) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 10) return mime_types[21];
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 60) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 33) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 68) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 79) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 67) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 84) {
              if (size == 6) return null;
              byte b = ((const byte *)data)[6];
              if (b == 89) {
                if (size == 7) return null;
                byte b = ((const byte *)data)[7];
                if (b == 80) {
                  if (size == 8) return null;
                  byte b = ((const byte *)data)[8];
                  if (b == 69) {
                    if (size == 9) return null;
                    byte b = ((const byte *)data)[9];
                    if (b == 32) {
                      if (size == 10) return null;
                      byte b = ((const byte *)data)[10];
                      if (b == 104) {
                        if (size == 11) return null;
                        byte b = ((const byte *)data)[11];
                        if (b == 116) {
                          if (size == 12) return null;
                          byte b = ((const byte *)data)[12];
                          if (b == 109) {
                            if (size == 13) return null;
                            byte b = ((const byte *)data)[13];
                            if (b == 108) {
                              if (size == 14) return null;
                              byte b = ((const byte *)data)[14];
                              if (b == 62) return mime_types[25];
                              return null;
                            }
                            return null;
                          }
                          return null;
                        }
                        return null;
                      }
                      return null;
                    }
                    return null;
                  }
                  return null;
                }
                return null;
              }
              return null;
            }
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 119) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 79) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 70) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 70) return mime_types[29];
        if (b == 50) return mime_types[30];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 55) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 122) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 188) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 175) {
          if (size == 4) return null;
          byte b = ((const byte *)data)[4];
          if (b == 39) {
            if (size == 5) return null;
            byte b = ((const byte *)data)[5];
            if (b == 28) return mime_types[34];
            return null;
          }
          return null;
        }
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 127) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 69) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 76) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 70) return mime_types[36];
        return null;
      }
      return null;
    }
    return null;
  }
  if (b == 77) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 90) return mime_types[37];
    return null;
  }
  if (b == 112) {
    if (size == 1) return null;
    byte b = ((const byte *)data)[1];
    if (b == 108) {
      if (size == 2) return null;
      byte b = ((const byte *)data)[2];
      if (b == 97) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 99) return mime_types[38];
        return null;
      }
      if (b == 102) {
        if (size == 3) return null;
        byte b = ((const byte *)data)[3];
        if (b == 102) return mime_types[39];
        return null;
      }
      return null;
    }
    return null;
  }
  return null;
}
