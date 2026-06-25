
import { Box, Button, Typography, Card, CardContent, Grid, Divider } from '@mui/material';

export default function ControlManual({ espStatus }) {
  // Función genérica para manejar el inicio y detención de cualquier subsistema
  const handleCommand = async (subsystemId, actionName, label) => {
    try {
      const response = await fetch('/api/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        // Enviamos la acción y el ID del subsistema al backend
        body: JSON.stringify({ action: actionName, subsystem: subsystemId })
      });
      if (response.ok) {
        console.log(`Señal de "${label}" enviada al Subsistema ${subsystemId}`);
      }
    } catch (error) {
      console.error(`Error enviando comando al subsistema ${subsystemId}:`, error);
    }
  };

  // Lista de subsistemas con sus botones independientes y acciones personalizadas
  const subsystems = [
    { 
      id: 1, 
      name: 'Mecanismo de arrastre',
      buttons: [
        { label: 'Avanzar', action: 'act_extend', color: 'success' },
        { label: 'Retroceder', action: 'act_retract', color: 'warning' },
        { label: 'Detener', action: 'act_stop', color: 'error' }
      ] 
    },
    { 
      id: 2, 
      name: 'Carro transbordador',
      buttons: [
        { label: 'Avanzar', action: 'carro_forward', color: 'success' },
        { label: 'Retroceder', action: 'carro_backward', color: 'warning' },
        { label: 'Detener', action: 'carro_stop', color: 'error' }
      ]
    },
    { 
      id: 3, 
      name: 'Estabilizadores',
      buttons: [
        { label: 'Desplegar', action: 'empot_forward', color: 'success' },
        { label: 'Retraer', action: 'empot_backward', color: 'warning' },
        { label: 'Detener', action: 'empot_stop', color: 'error' }
      ]
    },
    {
      id: 4,
      name: 'Sistema de enganche inicial',
      buttons: [
        { label: 'Enrollar', action: 'enganche_enrollar', color: 'success' },
        { label: 'Desenrollar', action: 'enganche_desenrollar', color: 'warning' },
        { label: 'Detener', action: 'enganche_stop', color: 'error' }
      ]
    }
  ];

  return (
    <Card sx={{ bgcolor: '#1a1a1a', border: '1px solid #333', mt: { xs: 0, lg: 4 } }}>
      <CardContent>
        <Typography variant="h6" sx={{ color: '#e0e0e0', mb: 3, fontWeight: 'bold' }}>
          CONTROL MANUAL DE SUBSISTEMAS
        </Typography>
        <Grid container spacing={3}>
          {subsystems.map((sub, index) => (
            <Grid item xs={12} key={sub.id}>
              <Box sx={{ display: 'flex', flexDirection: { xs: 'column', sm: 'row' }, alignItems: { xs: 'flex-start', sm: 'center' }, gap: 2 }}>
                <Typography sx={{ color: '#b0bec5', fontSize: '1.1rem', flex: 1, width: { xs: '100%', sm: '380px' } }}>{sub.name}</Typography>
                <Box sx={{ display: 'flex', gap: 2, width: { xs: '100%', sm: 'auto' }, flexShrink: 0, flexWrap: 'wrap' }}>
                  {sub.buttons.map((btn, btnIndex) => (
                    <Button 
                      key={btnIndex} 
                      sx={{ flex: 1, minWidth: '100px' }} 
                      variant="contained" 
                      color={btn.color} 
                      onClick={() => handleCommand(sub.id, btn.action, btn.label)} 
                      disabled={espStatus === 'offline'}
                    >
                      {btn.label}
                    </Button>
                  ))}
                </Box>
              </Box>
              {/* Divisor visual entre subsistemas, excepto en el último */}
              {index < subsystems.length - 1 && <Divider sx={{ mt: 3, borderColor: '#333' }} />}
            </Grid>
          ))}
        </Grid>
      </CardContent>
    </Card>
  );
}