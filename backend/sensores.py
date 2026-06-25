from flask import Blueprint, request, jsonify
import time

sensores_bp = Blueprint('sensors', __name__)

# Diccionario temporal para almacenar el estado de los sensores
sensors_data = {}

# Almacena el comando actual para el ESP32
pending_command = {"action": "idle"}

# Estado global del microcontrolador
esp32_status = {
    "last_check": 0,
    "status": "offline",
    "ping": 0
}

@sensores_bp.route('/command', methods=['POST'])
def send_command():
    global pending_command
    data = request.get_json()
    # Esperamos algo como {"action": "start"}
    pending_command = data 
    print(pending_command)
    return jsonify({"message": f"Comando {data['action']} registrado"}), 200

@sensores_bp.route('/update', methods=['POST'])
def update_sensor():
    data = request.get_json()
    
    # Validación de seguridad
    if not data:
        return jsonify({"error": "Invalid payload"}), 400
    
    for key, val in data.items():
        sensor_id = key  # Lo primero
        
        sensors_data[sensor_id] = {
            "value": val,  # Lo segundo
            "status": "active"
        }
        print(sensors_data)

    return jsonify({"message": "Data received", "status": "success"}), 200

@sensores_bp.route('/check', methods=['POST'])
def heartbeat():
    global pending_command
    data = request.get_json()

    # Actualizamos el timestamp exacto del servidor al recibir la petición
    esp32_status['last_check'] = time.time()
    esp32_status['status'] = "online"

    if data and 'ping' in data:
        esp32_status['ping'] = data.get('ping')

    response = {
        "message": "Heartbeat ACK",
        # Siempre envía el último estado conocido
        "command": pending_command.get('action', 'idle'),
        "subsystem": pending_command.get('subsystem', 0)
    }
    
    # BORRAMOS LA LÓGICA DE LIMPIEZA. 
    # El comando se mantiene activo hasta que React mande un "stop" u otra acción.
        
    return jsonify(response), 200


@sensores_bp.route('/status', methods=['GET'])
def get_sensors():
    return jsonify(sensors_data), 200

@sensores_bp.route('/system', methods=['GET'])
def get_system():
    # Si han pasado más de 5 segundos desde el último check, lo marcamos offline
    if time.time() - esp32_status['last_check'] > 5:
        esp32_status['status'] = "offline"
    return jsonify({
        "device": esp32_status,
        "sensors": sensors_data
    }), 200