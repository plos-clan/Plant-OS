from flask import Flask, request, jsonify
import webbrowser

app = Flask(__name__)


@app.route('/')
def index():
  return 'Hello, World!'


def start():
  webbrowser.open('http://localhost:5678/')
