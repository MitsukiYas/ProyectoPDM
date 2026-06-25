import { useState, useEffect } from 'react';
import ControlButtons from '../components/ControlButtons';
import CADView from '../CADView';
import SensorPanel from '../components/SensorPanel';
import ProcessStatus from '../components/ProcessStatus';
import ControlManual from '../components/ControlManual';

export default function Dashboard() {
  // 1. Estado centralizado para todo el sistema
  const [systemData, setSystemData] = useState({
    device: { status: 'offline', last_check: 0 },
    sensors: {}
  });

  const [showSensors, setShowSensors] = useState(false);

  // 2. Polling para obtener datos en tiempo real
  useEffect(() => {
    const fetchSystemData = async () => {
      try {
        const response = await fetch('/api/system');
        if (response.ok) {
          const data = await response.json();
          setSystemData(data);
        }
      } catch (error) {
        console.error("Error conectando con el backend:", error);
        setSystemData(prev => ({ ...prev, device: { status: 'offline' } }));
      }
    };

    fetchSystemData(); // Carga inicial
    const interval = setInterval(fetchSystemData, 2000); // Actualización cada 2 segundos

    return () => clearInterval(interval); // Limpieza al salir
  }, []);

  return (
    <div className="flex flex-col lg:flex-row gap-4 p-4 min-h-[calc(100vh-74px)] lg:h-[calc(100vh-74px)] w-full overflow-y-auto lg:overflow-hidden">
      
      {/* --- VISTA MÓVIL (Prioridad en Controles) --- */}
      <div className="flex flex-col gap-4 lg:hidden w-full">
        <ControlButtons espStatus={systemData.device.status} />
        <ControlManual espStatus={systemData.device.status} />

        <button
          onClick={() => setShowSensors(true)}
          className="fixed bottom-6 right-6 z-40 bg-[#2a2a2a] border border-[#444] rounded-full px-4 py-3 text-white shadow-lg"
        >
          Sensores
        </button>
      </div>

      {/* --- VISTA PC (Layout en columnas original) --- */}
      
      {/* Lado Izquierdo */}
      <div className="hidden lg:flex lg:w-[50%] flex-col gap-4">
        <CADView sensors={systemData.sensors} />
        <ControlButtons espStatus={systemData.device.status} />
      </div>

      {/* Lado Derecho */}
      <div className="hidden lg:flex lg:w-[50%] flex-col gap-4 h-full overflow-hidden">
        <div className="flex-1 overflow-y-auto pr-2 w-full">
          <SensorPanel sensors={systemData.sensors} device={systemData.device} />
        </div>
        <ControlManual espStatus={systemData.device.status} />
      </div>

      <div
        className={`
          fixed top-0 right-0 h-full w-[85vw]
          bg-[#111]
          z-50
          transform transition-transform duration-300
          ${showSensors ? "translate-x-0" : "translate-x-full"}
        `}
      >
        <div className="p-4 flex justify-between items-center border-b border-[#333]">
          <h2 className="text-white font-bold">
            Sensores
          </h2>

          <button
            onClick={() => setShowSensors(false)}
            className="text-gray-400"
          >
            ✕
          </button>
        </div>

        <div className="p-4 overflow-y-auto h-[calc(100%-60px)]">
          <SensorPanel
            sensors={systemData.sensors}
            device={systemData.device}
          />
        </div>
      </div>

      {showSensors && (
        <div
          className="fixed inset-0 bg-black/50 z-40"
          onClick={() => setShowSensors(false)}
        />
      )}

    </div>
  );
}