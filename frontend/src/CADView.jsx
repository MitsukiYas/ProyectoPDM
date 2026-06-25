export default function CADView({ sensors }) {
  // 1. Mapeo de datos (Ajusta los IDs según tus sensores reales)
  // Posición vertical del carro en los rieles (0% a 100%)
  const carPosition = sensors['S_POS']?.value || 35; 
  
  // Estado de los estabilizadores (Bloqueado/Desbloqueado)
  const isLocked = sensors['S_LOCK']?.value === 1; 
  
  // Extensión del brazo telescópico (0% a 100%)
  const armExtension = sensors['S_ARM']?.value || 0; 

  return (
    <div className="flex flex-col flex-1 min-h-[400px] bg-[#1a1a1a] rounded-xl border border-[#333] p-4 relative">
      <span className="block mb-4 text-sm font-medium tracking-wide text-gray-400 uppercase">
        DIAGRAMA DEL SISTEMA (VISTA DE PLANTA)
      </span>
      
      <div className="flex-1 bg-[#121212] border border-[#333] rounded-lg relative overflow-hidden">
        {/* Rieles Principales */}
        <div className="absolute top-0 bottom-0 left-[30%] w-3 bg-[#2c3e50]"></div>
        <div className="absolute top-0 bottom-0 left-[70%] w-3 bg-[#2c3e50]"></div>

        {/* --- CONJUNTO DEL CARRO TRANSBORDADOR --- */}
        {/* Usamos inline style para la posición dinámica y transición para suavidad */}
        <div 
          className="absolute left-[15%] right-[15%] h-40 flex items-center justify-center transition-all duration-700 ease-in-out"
          style={{ top: `${carPosition}%` }} 
        >
          
          {/* Estabilizador Izquierdo */}
          <div className={`w-10 h-10 border-2 rounded-l-md flex items-center justify-center z-10 relative left-1 transition-colors duration-300 ${isLocked ? 'bg-[#455a64] border-[#4caf50]' : 'bg-[#263238] border-[#333]'}`}>
            <div className={`w-3 h-3 rounded-full transition-all ${isLocked ? 'bg-[#4caf50] shadow-[0_0_8px_#4caf50]' : 'bg-[#555]'}`}></div>
          </div>

          {/* Cuerpo Principal */}
          <div className="flex-1 h-full bg-[#37474f] border-4 border-[#546e7a] rounded-lg relative z-20 flex flex-col items-center shadow-2xl">
            <span className="text-white text-[10px] font-semibold mt-4 tracking-widest uppercase">Carro Transbordador</span>
            
            {/* Indicador de Bloqueo */}
            <div className={`flex items-center gap-2 mt-2 px-3 py-1.5 rounded-md border transition-colors ${isLocked ? 'bg-[#263238] border-[#4caf50]' : 'bg-[#1a1a1a] border-red-900'}`}>
              <div className={`w-2 h-2 rounded-full ${isLocked ? 'bg-[#4caf50] animate-pulse' : 'bg-red-600'}`}></div>
              <span className={`text-[9px] font-bold tracking-widest uppercase ${isLocked ? 'text-[#4caf50]' : 'text-red-600'}`}>
                {isLocked ? 'Bloqueado' : 'Movimiento Libre'}
              </span>
            </div>

            {/* Mecanismo de Arrastre Telescópico */}
            <div className="absolute top-[65%] left-[5%] w-[130%] h-10 flex items-center">
              <div className="w-[45%] h-full bg-[#d35400] border-2 border-[#e65100] z-30 flex items-center justify-center">
                <span className="text-white text-[8px] font-bold uppercase">Arrastre</span>
              </div>
              
              {/* Brazo Dinámico: El ancho depende de armExtension */}
              <div 
                className="h-6 bg-[#e67e22] border-y-2 border-r-2 border-[#d35400] z-20 flex items-center justify-end pr-3 transition-all duration-500 overflow-hidden"
                style={{ width: `${armExtension}%` }}
              >
                {armExtension > 20 && (
                  <div className="flex text-white font-black text-lg">
                    <span className="animate-pulse">&gt;</span>
                    <span className="animate-pulse" style={{ animationDelay: '0.2s' }}>&gt;</span>
                  </div>
                )}
              </div>
            </div>
          </div>

          {/* Estabilizador Derecho */}
          <div className={`w-10 h-10 border-2 rounded-r-md flex items-center justify-center z-10 relative right-1 transition-colors ${isLocked ? 'bg-[#455a64] border-[#4caf50]' : 'bg-[#263238] border-[#333]'}`}>
            <div className={`w-3 h-3 rounded-full ${isLocked ? 'bg-[#4caf50] shadow-[0_0_8px_#4caf50]' : 'bg-[#555]'}`}></div>
          </div>
        </div>

        {/* Luces de riel reactivas */}
        {isLocked && (
          <>
            <div className="absolute top-0 bottom-0 left-[30%] w-3 border-x border-[#4caf50] opacity-20 bg-[#4caf50]/10"></div>
            <div className="absolute top-0 bottom-0 left-[70%] w-3 border-x border-[#4caf50] opacity-20 bg-[#4caf50]/10"></div>
          </>
        )}
      </div>
    </div>
  );
}