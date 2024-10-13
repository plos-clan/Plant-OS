#!/usr/bin/python3
from flask import Flask, render_template, send_file
from flask_socketio import SocketIO, send, emit
import sys
import re
import time
import threading
import eventlet
import os

thread_event = threading.Event()

app = Flask(__name__)
socketio = SocketIO(app, async_mode='eventlet')

logs = []
log_stats = {'total': 0, 'types': {}, 'files': {}, 'funcs': {}}


def update_stats(log_type, log_file, log_func):
  log_stats['total'] += 1
  if log_type in log_stats['types']:
    log_stats['types'][log_type] += 1
  else:
    log_stats['types'][log_type] = 1
  if log_file in log_stats['files']:
    log_stats['files'][log_file] += 1
  else:
    log_stats['files'][log_file] = 1
  if log_func in log_stats['funcs']:
    log_stats['funcs'][log_func] += 1
  else:
    log_stats['funcs'][log_func] = 1


def read_logs():
  while True:
    line = sys.stdin.readline()
    line = re.sub(r'\033\[.*?m', '', line)
    if not line:
      time.sleep(0.1)
      continue
    match = re.search(r'\[(.*?)\] *\[(.*?)\] *\[(.*?):(.*?)\]', line)
    log_type = match.group(1).strip() if match else 'Unknown'
    log_file = match.group(2).strip() if match else 'Unknown'
    log_func = match.group(3).strip() if match else 'Unknown'
    log_line = match.group(4).strip() if match else 'Unknown'
    logs.append({'type': log_type, 'file': log_file, 'func': log_func, 'line': log_line, 'content': line.strip()})
    update_stats(log_type, log_file, log_func)
    thread_event.set()


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
  emit('logs', logs)
  emit('stats', log_stats)


def background_thread():
  while True:
    if thread_event.is_set():
      socketio.emit('logs', logs)
      socketio.emit('stats', log_stats)
      thread_event.clear()
    socketio.sleep(1)


if __name__ == '__main__':
  threading.Thread(target=read_logs, daemon=True).start()
  socketio.start_background_task(background_thread)
  socketio.run(app, port=1234, use_reloader=False, debug=False)
