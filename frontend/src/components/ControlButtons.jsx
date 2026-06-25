import { Box, Button } from '@mui/material';

export default function ControlButtons({ espStatus }) {
  
  // Función genérica para enviar comandos al backend
  const sendCommand = async (actionName) => {
    try {
      const response = await fetch('/api/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ action: actionName })
      });
      if (response.ok) {
        console.log(`Comando enviado con éxito: ${actionName}`);
      }
    } catch (error) {
      console.error(`Error enviando comando ${actionName}:`, error);
    }
  };

  return (
    <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3, mt: { xs: 0, lg: 4 }, width: '100%' }}>
      
      {/* =========================================================
          CONTROLES MAESTROS DEL AUTOMÁTICO
         ========================================================= */}
      <Box sx={{ display: 'flex', flexDirection: { xs: 'column', md: 'row' }, gap: 3 }}>
        <Button 
          variant="contained" 
          color="success" 
          size="large" 
          fullWidth 
          onClick={() => sendCommand('start_all')}
          disabled={espStatus === 'offline'}
          sx={{ py: 2.5, fontSize: '1.2rem', fontWeight: 'bold', letterSpacing: 2 }}
        >
          INICIAR PROCESO
        </Button>

        <Button 
          variant="contained" 
          color="error" 
          size="large" 
          fullWidth 
          onClick={() => sendCommand('stop_all')}
          disabled={espStatus === 'offline'}
          sx={{ py: 2.5, fontSize: '1.2rem', fontWeight: 'bold', letterSpacing: 2 }}
        >
          DETENER PROCESO
        </Button>
      </Box>

      {/* =========================================================
          SECCIÓN DE CONFIRMACIÓN (Box actúa como contenedor y texto)
         ========================================================= */}
      <Box 
        sx={{ 
          bgcolor: '#1a1a1a', 
          border: '1px solid #333', 
          p: 3, 
          borderRadius: 2 
        }}
      >
        {/* Box estilizado para actuar como la etiqueta de Typography */}
        <Box 
          sx={{ 
            color: '#888', 
            fontSize: '0.75rem', 
            fontWeight: 'bold', 
            mb: 2, 
            letterSpacing: 1, 
            textTransform: 'uppercase' 
          }}
        >
          CONFIRMACIONES DEL OPERADOR (PASOS PAUSADOS)
        </Box>
        
        {/* Box estructurado con flexbox para reemplazar al Grid */}
        <Box sx={{ display: 'flex', flexDirection: { xs: 'column', sm: 'row' }, gap: 2 }}>
          <Button
            variant="outlined"
            color="info"
            fullWidth
            onClick={() => sendCommand('enganche_listo')}
            disabled={espStatus === 'offline'}
            sx={{ 
              py: 1.5, 
              color: '#2196f3', 
              borderColor: '#2196f3',
              fontWeight: 'medium',
              '&:hover': { borderColor: '#64b5f6', bgcolor: 'rgba(33, 150, 243, 0.04)' }
            }}
          >
            1. Cuerda Colocada
          </Button>
          
          <Button
            variant="outlined"
            color="info"
            fullWidth
            onClick={() => sendCommand('enganche_desacoplado')}
            disabled={espStatus === 'offline'}
            sx={{ 
              py: 1.5, 
              color: '#b0bec5', 
              borderColor: '#455a64',
              fontWeight: 'medium',
              '&:hover': { borderColor: '#90a4ae', bgcolor: 'rgba(144, 164, 174, 0.04)' }
            }}
          >
            2. Enganche Liberado
          </Button>
        </Box>
      </Box>

    </Box>
  );
}