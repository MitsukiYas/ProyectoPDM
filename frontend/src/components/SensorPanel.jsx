const statusStyles = {
  'active': { text: 'text-[#4caf50]', dot: 'bg-[#4caf50] shadow-[0_0_8px_rgba(76,175,80,0.4)]' },
  'warning': { text: 'text-[#ffeb3b]', dot: 'bg-[#ffeb3b] shadow-[0_0_8px_rgba(255,235,59,0.4)]' },
  'error': { text: 'text-[#f44336]', dot: 'bg-[#f44336] shadow-[0_0_8px_rgba(244,67,54,0.4)]' },
  'offline': { text: 'text-[#9e9e9e]', dot: 'bg-[#9e9e9e] shadow-[0_0_8px_rgba(158,158,158,0.4)]' }
};

const nombres = {
    carro_cap: "Sensor capacitivo del carro",
    empot1: "Fin de carrera estabilizador izquierdo",
    empot2: "Fin de carrera estabilizador derecho",
    enganche: "Fin de carrera del enganche",
    actuador: "Posición del actuador"
};

const formatValue = (id, value) => {
  console.log(id, value);
  if (id === "actuador"){
    return value ? `${value} mm` : "No se detectó";
  }
  
  if (
      id === "carro_cap" ||
      id === "empot1" ||
      id === "empot2" ||
      id === "enganche"
  ) {
    return value ? "Activado" : "Desactivado";
  }

  return value;
};



// Recibimos sensors y device como props
export default function SensorPanel({ sensors, device }) {
  // Convertimos el objeto de sensores a un array para poder usar .map()
  const sensorsList = Object.entries(sensors).map(([id, data]) => ({
    id,
    ...data
  }));

  // Función para determinar el color del ping según la latencia
  const getPingStyle = (ms) => {
    if (ms < 100) return 'text-[#4caf50]'; // Bueno
    if (ms < 300) return 'text-[#ffeb3b]'; // Regular
    return 'text-[#f44336]'; // Malo
  };

  return (
    <div className="bg-[#1a1a1a] rounded-xl border border-[#333] w-full">
      <div className="p-3 lg:p-4">
        <div className="mb-4 flex justify-between items-center">
          <span className="block text-sm font-medium tracking-wide text-gray-400 uppercase">
            SENSORES DEL SISTEMA
          </span>
          {/* VISUALIZACIÓN DEL PING */}
          <div className="flex items-center gap-2">
            <span className="text-[10px] text-gray-500 uppercase font-bold">Ping:</span>
            <span className={`text-xs font-mono font-bold ${getPingStyle(device.ping || 0)}`}>
              {device.ping || 0}ms
            </span>
          </div>
        </div>
        <hr className="border-[#333] mb-4"/>

        <div className="flex flex-col">
          {/* Si no hay sensores todavía */}
          {sensorsList.length === 0 && (
            <span className="text-gray-600 text-xs italic py-4 text-center">
              Esperando datos del ESP32...
            </span>
          )}

          {sensorsList.map((sensor, index) => {
            const styles = statusStyles[sensor.status] || statusStyles['offline'];
            return (
              <div key={sensor.id} className="w-full">
                <div className="flex flex-row justify-between items-center py-3">
                  <div className="flex items-center gap-4">
                    <div className={`w-2.5 h-2.5 rounded-full ${styles.dot}`} />
                    <div className="flex flex-col">
                      <span className="text-sm font-medium text-[#e0e0e0]">{nombres[sensor.id] || sensor.id}</span>
                    </div>
                  </div>
                  <div className="flex items-center">
                    <span className={`text-sm font-bold ${styles.text} tabular-nums`}>
                      {formatValue(sensor.id, sensor.value)}
                    </span>
                  </div>
                </div>
                {index < sensorsList.length - 1 && <hr className="border-[#333]" />}
              </div>
            );
          })}
        </div>
      </div>
    </div>
  );
}