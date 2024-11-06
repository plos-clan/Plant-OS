
#include <define.h>

enum {
  MIME_UNKNOWN,
  MIME_TEXT,
  MIME_IMAGE,
  MIME_AUDIO,
  MIME_VIDEO,
  MIME_APPLICATION,
  MIME_FONT,
  MIME_MESSAGE,
  MIME_MODEL,
  MIME_MULTIPART,
};

static bool binary_equal(const void *_s1, const void *_s2, size_t n) {
  const byte *s1 = _s1;
  const byte *s2 = _s2;
  for (size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i]) return false;
  }
  return true;
}

static i32 match_count(const void *_text, i32 textlen, const void *data) {
  const byte *text       = (const byte *)_text;
  i32         patternlen = *(const i32 *)data;
  const byte *pattern    = (const byte *)data + 4;
  const i32  *lps        = (const i32 *)data + 1 + (patternlen + 3) / 4;
  i32         count      = 0;
  for (i32 i = 0, j = 0; i < textlen; i++) {
    if (pattern[j] == text[i]) {
      j++;
    } else if (j != 0) {
      j = lps[j - 1];
      i--;
    }
    if (j == patternlen) {
      count++;
      j = 0;
    }
  }
  return count;
}

static const struct filetype {
  const i32  id;
  const i32  type;
  const cstr mime;
  const cstr desc;
} filetypes[] = {
    {0,  MIME_APPLICATION, "application/java-archive",     "Java Archive file"                  },
    {1,  MIME_APPLICATION, "application/msword",           "Microsoft Word document"            },
    {2,  MIME_APPLICATION, "application/pdf",              "Portable Document Format"           },
    {3,  MIME_APPLICATION, "application/vnd.amazon.ebook", "Amazon Kindle eBook file"           },
    {4,  MIME_APPLICATION, "application/vnd.ms-excel",     "Microsoft Excel document"           },
    {5,  MIME_APPLICATION, "application/wasm",             "WebAssembly binary file"            },
    {6,  MIME_APPLICATION, "application/x-bzip",           "Bzip archive file"                  },
    {7,  MIME_APPLICATION, "application/x-bzip2",          "Bzip2 archive file"                 },
    {8,  MIME_APPLICATION, "application/x-cdf",            "Common Data Format file"            },
    {9,  MIME_APPLICATION, "application/x-csh",            "Bash script"                        },
    {10, MIME_APPLICATION, "application/x-rar-compressed", "RAR archive file"                   },
    {11, MIME_APPLICATION, "application/zip",              "ZIP archive file"                   },
    {12, MIME_AUDIO,       "audio/aac",                    "Advanced Audio Coding file"         },
    {13, MIME_AUDIO,       "audio/mpeg",                   "MP3 audio file"                     },
    {14, MIME_AUDIO,       "audio/ogg",                    "Ogg Vorbis audio file"              },
    {15, MIME_AUDIO,       "audio/wav",                    "Waveform Audio file"                },
    {16, MIME_AUDIO,       "audio/webm",                   "WebM audio file"                    },
    {17, MIME_AUDIO,       "audio/x-qoa",                  "Quite OK Audio file"                },
    {18, MIME_IMAGE,       "image/gif",                    "Graphics Interchange Format file"   },
    {19, MIME_IMAGE,       "image/apng",                   "Animated Portable Network Graphics" },
    {20, MIME_IMAGE,       "image/jpeg",                   "JPEG image file"                    },
    {21, MIME_IMAGE,       "image/png",                    "Portable Network Graphics"          },
    {22, MIME_IMAGE,       "image/vnd.microsoft.icon",     "Icon file"                          },
    {23, MIME_TEXT,        "text/css",                     "Cascading Style Sheets file"        },
    {24, MIME_TEXT,        "text/csv",                     "Comma-separated values file"        },
    {25, MIME_TEXT,        "text/html",                    "Hypertext Markup Language file"     },
    {26, MIME_VIDEO,       "video/mp4",                    "MPEG-4 video file"                  },
    {27, MIME_VIDEO,       "video/x-msvideo",              "Audio Video Interleave file"        },
    {28, MIME_VIDEO,       "video/webm",                   "WebM video file"                    },
    {29, MIME_FONT,        "font/woff",                    "Web Open Font Format file"          },
    {30, MIME_FONT,        "font/woff2",                   "Web Open Font Format 2 file"        },
    {31, MIME_FONT,        "font/ttf",                     "TrueType Font file"                 },
    {32, MIME_FONT,        "font/otf",                     "OpenType Font file"                 },
    {33, MIME_IMAGE,       "image/svg+xml",                "Scalable Vector Graphics file"      },
    {34, MIME_APPLICATION, "application/x-7z-compressed",  "7-Zip archive file"                 },
    {35, MIME_TEXT,        "text/x-script",                "Script file"                        },
    {36, MIME_APPLICATION, "application/x-executable",     "Executable and Linkable Format file"},
    {37, MIME_APPLICATION, "application/x-msdownload",     "Windows Executable file"            },
    {38, MIME_AUDIO,       "audio/x-plac",                 "Plant-OS audio file"                },
    {39, MIME_FONT,        "font/x-plff",                  "Plant-OS font file"                 },
};

static const struct {
  const i32   id;
  const i32   len;
  const void *magic;
} magic_numbers[] = {
    {0,  4,  "\x50\x4b\x03\x04"                                },
    {1,  8,  "\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1"                },
    {2,  5,  "%PDF-"                                           },
    {3,  4,  "\x50\x4b\x03\x04"                                },
    {4,  8,  "\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1"                },
    {5,  8,  "\x00\x61\x73\x6d\x01\x00\x00\x00"                },
    {6,  3,  "BZh"                                             },
    {7,  3,  "BZh"                                             },
    {8,  4,  "CD00"                                            },
    {9,  9,  "#!/bin/sh"                                       },
    {9,  15, "#!/usr/bin/bash"                                 },
    {10, 7,  "\x52\x61\x72\x21\x1a\x07\x00"                    },
    {11, 4,  "\x50\x4b\x03\x04"                                },
    {12, 2,  "\xff\xf1"                                        },
    {13, 2,  "\xff\xfb"                                        },
    {14, 4,  "OggS"                                            },
    {15, 4,  "RIFF"                                            },
    {16, 4,  "\x1a\x45\xdf\xa3"                                },
    {17, 4,  "qoaf"                                            },
    {18, 6,  "GIF89a"                                          },
    {19, 8,  "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"                },
    {20, 3,  "\xff\xd8\xff"                                    },
    {21, 8,  "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"                },
    {22, 4,  "\x00\x00\x01\x00"                                },
    {25, 15, "<!DOCTYPE html>"                                 },
    {26, 12, "\x00\x00\x00\x18\x66\x74\x79\x70\x6d\x70\x34\x32"},
    {27, 4,  "RIFF"                                            },
    {28, 4,  "\x1a\x45\xdf\xa3"                                },
    {29, 4,  "wOFF"                                            },
    {30, 4,  "wOF2"                                            },
    {31, 4,  "\x00\x01\x00\x00"                                },
    {32, 4,  "OTTO"                                            },
    {34, 6,  "\x37\x7a\xbc\xaf\x27\x1c"                        },
    {35, 2,  "#!"                                              },
    {36, 4,  "\x7f\x45\x4c\x46"                                },
    {37, 2,  "MZ"                                              },
    {38, 4,  "plac"                                            },
    {39, 4,  "plff"                                            },
};

static const struct {
  const i32   id;
  const i32   len;
  const void *ext;
} file_extensions[] = {
    {0,  4, "jar"  },
    {1,  4, "doc"  },
    {2,  4, "pdf"  },
    {3,  4, "azw"  },
    {4,  4, "xls"  },
    {5,  5, "wasm" },
    {6,  3, "bz"   },
    {7,  4, "bz2"  },
    {8,  4, "cdf"  },
    {9,  4, "csh"  },
    {9,  3, "sh"   },
    {10, 4, "rar"  },
    {11, 4, "zip"  },
    {12, 4, "aac"  },
    {13, 4, "mp3"  },
    {14, 4, "ogg"  },
    {15, 4, "wav"  },
    {16, 5, "weba" },
    {17, 4, "qoa"  },
    {18, 4, "gif"  },
    {19, 5, "apng" },
    {20, 5, "jpeg" },
    {20, 4, "jpg"  },
    {21, 4, "png"  },
    {22, 4, "ico"  },
    {23, 4, "css"  },
    {24, 4, "csv"  },
    {25, 5, "html" },
    {25, 4, "htm"  },
    {26, 4, "mp4"  },
    {27, 4, "avi"  },
    {28, 5, "webm" },
    {29, 5, "woff" },
    {30, 6, "woff2"},
    {31, 4, "ttf"  },
    {32, 4, "otf"  },
    {33, 4, "svg"  },
    {34, 3, "7z"   },
    {36, 4, "elf"  },
    {37, 4, "exe"  },
    {38, 5, "plac" },
    {39, 5, "plff" },
};

dlexport cstr filetype(const void *data, size_t size) {
  for (size_t i = 0; i < lengthof(magic_numbers); i++) {
    if (size < magic_numbers[i].len) continue;
    if (binary_equal(data, magic_numbers[i].magic, magic_numbers[i].len)) {
      return filetypes[magic_numbers[i].id].mime;
    }
  }
  return null;
}

dlexport cstr filetype_by_ext(cstr ext) {
  for (size_t i = 0; i < lengthof(file_extensions); i++) {
    if (binary_equal(ext, file_extensions[i].ext, file_extensions[i].len)) {
      return filetypes[file_extensions[i].id].mime;
    }
  }
  return null;
}
