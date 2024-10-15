#!/usr/bin/python3
from flask import Flask, render_template, send_file
from flask_socketio import SocketIO, send, emit
import sys
import re
import time
import threading
import eventlet
import os
import struct

# 'Plant-OS' 'Plant-OS ext'
LOG_TYPE = 'Plant-OS'

PORT = 1234

thread_event = threading.Event()

app = Flask(__name__)
socketio = SocketIO(app, async_mode='eventlet')

logs: list[dict] = []
log_stats = {
    'total': 0,
    'types': {},
    'files': {},
    'funcs': {},
    'cpus': {},
    'threads': {},
}
lod_default_data = {
    'time': -1,
    'type': 'Unknown',
    'file': 'Unknown',
    'func': 'Unknown',
    'line': -1,
    'cpu': -1,
    'thread': -1,
    'content': '',
}
log_cols = [k for k in lod_default_data.keys()]


def read(size: int):
  data = sys.stdin.buffer.read(size)
  if len(data) < 4: exit(0)
  return data


def read8():
  return struct.unpack('b', read(1))[0]


def read16():
  return struct.unpack('h', read(2))[0]


def read32():
  return struct.unpack('i', read(4))[0]


def read64():
  return struct.unpack('q', read(8))[0]


def reads():
  return read(read32()).decode('utf-8')


def log_update(in_data: dict):
  data = dict(lod_default_data)
  data.update(in_data)
  if data['time'] < 0: data['time'] = int(time.time())
  print(data)
  log_stats['total'] += 1
  for key in ['type', 'file', 'func', 'cpu', 'thread']:
    if data[key] in log_stats[f'{key}s']:
      log_stats[f'{key}s'][data[key]] += 1
    else:
      log_stats[f'{key}s'][data[key]] = 1
  logs.append(data)
  thread_event.set()


def read_log_plos():
  line = sys.stdin.readline()
  line = re.sub(r'\033\[.*?m', '', line)
  if not line or not line.strip():
    time.sleep(0.1)
    return
  match = re.search(r'\[(.*?)\] *\[(.*?)\] *\[(.*?):(.*?)\] *(.*)', line)
  log_update({
      'type': match.group(1).strip() if match else 'Unknown',
      'file': match.group(2).strip() if match else 'Unknown',
      'func': match.group(3).strip() if match else 'Unknown',
      'line': int(match.group(4).strip()) if match else -1,
      'cpu': 0,
      'content': match.group(5).strip(),
  })


# [  OK ]
# [FALED]


def read_log_plos_ext():
  id = read32()
  if id == 0:
    log_update({
        'time': read32(),
        'type': reads(),
        'file': reads(),
        'func': reads(),
        'line': read32(),
        'cpu': read32(),
        'thread': read32(),
        'content': reads(),
    })
    return


def read_log_cpos():
  line = sys.stdin.readline()
  line = re.sub(r'\033\[.*?m', '', line)
  if not line or not line.strip():
    time.sleep(0.1)
    return
  match = re.search(r'\[(.*?)\] *\[(.*?)\] *\[(.*?):(.*?)\] *(.*)', line)
  log_update({
      'time': int(time.time()),
      'type': match.group(1).strip() if match else 'Unknown',
      'file': match.group(2).strip() if match else 'Unknown',
      'func': match.group(3).strip() if match else 'Unknown',
      'line': int(match.group(4).strip()) if match else -1,
      'cpu': 0,
      'content': match.group(5).strip(),
  })


read_log_funcs = {
    'Plant-OS': read_log_plos,
    'Plant-OS ext': read_log_plos_ext,
    'CoolPot-OS': read_log_cpos,
}


@app.route('/')
def index():
  return render_template('index.html')


@app.route('/<path:path>')
def catch_all(path):
  path = 'templates/' + path
  if os.path.exists(path):
    return send_file(path)
  return '', 404


@socketio.on('connect')
def handle_connect():
  emit('cols', log_cols)
  emit('logs', logs)
  emit('stats', log_stats)


def read_thread():
  while True:
    try:
      read_log_funcs[LOG_TYPE]()
    except:
      pass


def background_thread():
  while True:
    if thread_event.is_set():
      socketio.emit('logs', logs)
      socketio.emit('stats', log_stats)
      thread_event.clear()
    socketio.sleep(1)


if __name__ == '__main__':
  threading.Thread(target=read_thread, daemon=True).start()
  socketio.start_background_task(background_thread)
  socketio.run(app, port=PORT, use_reloader=False, debug=False)
