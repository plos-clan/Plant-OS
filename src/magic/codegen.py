#!/usr/bin/python3
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

next_typeid = 0

# 'unknown':     'MIME_UNKNOWN,    ',
# 'text':        'MIME_TEXT,       ',
# 'image':       'MIME_IMAGE,      ',
# 'audio':       'MIME_AUDIO,      ',
# 'video':       'MIME_VIDEO,      ',
# 'application': 'MIME_APPLICATION,',
# 'font':        'MIME_FONT,       ',
# 'message':     'MIME_MESSAGE,    ',
# 'model':       'MIME_MODEL,      ',
# 'multipart':   'MIME_MULTIPART,  ',

mime_enums = {
    'unknown': 'MIME_UNKNOWN,    ',
    'text': 'MIME_TEXT,       ',
    'image': 'MIME_IMAGE,      ',
    'audio': 'MIME_AUDIO,      ',
    'video': 'MIME_VIDEO,      ',
    'application': 'MIME_APPLICATION,',
    'font': 'MIME_FONT,       ',
    'message': 'MIME_MESSAGE,    ',
    'model': 'MIME_MODEL,      ',
    'multipart': 'MIME_MULTIPART,  ',
}


class MimeData:

  def __init__(self, mime):
    global next_typeid
    self.id = next_typeid
    self.mime = mime
    self.mime_maj = mime.split('/')[0]
    self.mime_min = mime.split('/')[1]
    self.magic: list[bytes] = []
    self.ext = []
    self.des = None
    next_typeid += 1

  def __str__(self):
    color0 = '\033[m'
    color1 = '\033[1;33;40m'
    color2 = '\033[1;32;40m'
    color3 = '\033[1;34;40m'
    color4 = '\033[1;35;40m'
    return f"{color1}{self.mime}{color0};{color2}{self.magic}{color0};{color3}{self.ext}{color0};{color4}{self.des}{color0}"


mimes: dict[str, MimeData] = {}

lines = []

with open('magic.csv', 'r') as f:
  for line in f:
    if len(line.strip()) == 0: continue
    lines.append(line.strip())

with open('magic.plos.csv', 'r') as f:
  for line in f:
    if len(line.strip()) == 0: continue
    lines.append(line.strip())

for line in lines:
  if len(line.strip()) == 0: continue
  line = line.strip().split(';')
  if line[0] == 'MIME': continue
  mime = line[0]
  type = line[1]
  magic = line[2]
  ext = line[3]
  des = line[4]
  if mime not in mimes: mimes[mime] = MimeData(mime)
  data = mimes[mime]
  if not type and magic: raise "error"
  if type and not magic: raise "error"
  if type == 'str': data.magic.append(magic.encode())
  if type == 'hex': data.magic.append(bytes.fromhex(magic))
  if ext: data.ext += ext.split(',')
  if des: data.des = des

mimes_list: list[MimeData] = [None] * next_typeid

for data in mimes.values():
  mimes_list[data.id] = data

code = '''
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
$$ filetypes $$
};

static const struct {
  const i32   id;
  const i32   len;
  const void *magic;
} magic_numbers[] = {
$$ list-magic-numbers $$
};

static const struct {
  const i32   id;
  const i32   len;
  const void *ext;
} file_extensions[] = {
$$ list-file-extensions $$
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
'''


def encode_bytes(data: bytes):
  use_escape = [byte < 32 or byte >= 127 for byte in data].count(True) > 0
  return ''.join([f'\\x{byte:02x}' if use_escape else chr(byte) for byte in data])


def gen_list_filetypes():
  code = []
  id_padding = max([len(repr(data.id)) for data in mimes_list])
  mime_padding = max([len(data.mime) for data in mimes_list])
  desc_padding = max([len(data.des) for data in mimes_list])
  for data in mimes_list:
    id_p = " " * (id_padding - len(repr(data.id)))
    mime_p = " " * (mime_padding - len(data.mime))
    desc_p = " " * (desc_padding - len(data.des))
    code.append(f'{{{data.id},{id_p} {mime_enums[data.mime_maj]} "{data.mime}",{mime_p} "{data.des}"{desc_p}}},')
  return '\n'.join(['    ' + s for s in code])


def gen_list_magic_numbers():
  code = []
  id_padding = max([len(repr(data.id)) for data in mimes_list])
  len_padding = max([len(repr(len(magic))) for data in mimes_list for magic in data.magic])
  magic_padding = max([len(encode_bytes(magic)) for data in mimes_list for magic in data.magic])
  for data in mimes_list:
    for magic in data.magic:
      id_p = " " * (id_padding - len(repr(data.id)))
      len_p = " " * (len_padding - len(repr(len(magic))))
      magic_p = " " * (magic_padding - len(encode_bytes(magic)))
      code.append(f'{{{data.id},{id_p} {len(magic)},{len_p} "{encode_bytes(magic)}"{magic_p}}},')
  return '\n'.join(['    ' + s for s in code])


def gen_list_file_extensions():
  code = []
  id_padding = max([len(repr(data.id)) for data in mimes_list])
  len_padding = max([len(repr(len(ext) + 1)) for data in mimes_list for ext in data.ext])
  ext_padding = max([len(ext) for data in mimes_list for ext in data.ext])
  for data in mimes_list:
    for ext in data.ext:
      id_p = " " * (id_padding - len(repr(data.id)))
      len_p = " " * (len_padding - len(repr(len(ext) + 1)))
      ext_p = " " * (ext_padding - len(ext))
      code.append(f'{{{data.id},{id_p} {len(ext) + 1},{len_p} "{ext}"{ext_p}}},')
  return '\n'.join(['    ' + s for s in code])


code = code.replace('$$ filetypes $$', gen_list_filetypes())
code = code.replace('$$ list-magic-numbers $$', gen_list_magic_numbers())
code = code.replace('$$ list-file-extensions $$', gen_list_file_extensions())

with open('filetype.c', 'w') as f:
  f.write(code)
