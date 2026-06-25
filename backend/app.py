from flask import Flask, jsonify, request
from flask_cors import CORS
from sensores import sensores_bp

app = Flask(__name__)
CORS(app)

# Registrar el blueprint
app.register_blueprint(sensores_bp, url_prefix='/api')

if __name__ == '__main__':
    #0.0.0.0 para que el ESP32 pueda encontrarlo en la red local
    app.run(host='0.0.0.0', port=5000, debug=True)




