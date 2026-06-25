import { processStateData } from '../mockData';

export default function ProcessStatus() {
  return (
    <div className="bg-[#1a1a1a] rounded-xl border border-[#333]">
      <div className="p-4">
        <span className="block mb-4 text-sm font-medium tracking-wide text-gray-400 uppercase">
          ESTADO DEL PROCESO
        </span>
        <div className="grid grid-cols-2 gap-4">
          <div>
            <span className="block text-xs text-gray-400">Modo de Operación</span>
            <span className="block text-base text-[#e0e0e0] truncate">{processStateData.mode}</span>
          </div>
          <div>
            <span className="block text-xs text-gray-400">Autoclaves Procesados</span>
            <span className="block text-base text-[#e0e0e0] truncate">{processStateData.numberAutoclavesProcess}</span>
          </div>
          <div>
            <span className="block text-xs text-gray-400">Autclaves restantes</span>
            <span className="block text-base text-[#e0e0e0] truncate">2</span>
          </div>
          <div className="col-span-2 mt-1">
            <span className="block text-xs text-gray-400">Estado Actual</span>
            <span className="block text-s font-medium text-[#64b5f6] mt-1">{processStateData.currentState}</span>
          </div>
          <div className="col-span-2 mt-1">
            <span className="block text-xs text-gray-400">Monitoreo del Sistema</span>
            <span className={`block text-base font-medium mt-1 ${processStateData.systemStatus.includes('ADVERTENCIA') ? 'text-[#ffeb3b]' : 'text-[#4caf50]'}`}>
              {processStateData.systemStatus}
            </span>
          </div>
        </div>
      </div>
    </div>
  );
}