export const sensorsData = [
  { name: 'Avance del carro', value: '70 cm', status: 'OK' },
  { name: 'Velocidad del carro', value: '0 m/s', status: 'OK' },
  { name: 'Conexión con ESP32', value: 'Ping: 12ms', status: 'OK' }
];

export const processStateData = {
  mode: 'Automático',
  numberAutoclavesProcess: '2',
  currentState: 'Alineando carro de transferencia',
  systemStatus: 'Sin problemas detectados'
};

export const faultsData = [
  { 
    timestamp: '2026-05-11 22:30:15', 
    type: 'ALINEACIÓN', 
    description: 'Desviación en riel de autoclave detectada (2mm)', 
    severity: 'ADVERTENCIA' 
  },
  { 
    timestamp: '2026-05-11 14:15:00', 
    type: 'MECÁNICA', 
    description: 'Fricción inusual en mecanismo de arrastre', 
    severity: 'CRÍTICO' 
  }
];