import { AppBar, Toolbar, Typography, Box, Button, Chip } from '@mui/material';
import {Link, useLocation} from 'react-router-dom';
import { useState, useEffect } from 'react';

export default function Navbar() {
  const location = useLocation();
  const [esp32Status, setEsp32Status] = useState("Desconectado");
  
  useEffect(() => {
    const fetchEsp32Status = async () => {
      try {
        const response = await fetch('/api/system');
        if (response.ok) {
          const data = await response.json();
          setEsp32Status(data.device.status === "online" ? "Conectado" : "Desconectado");
          console.log('ESP32 Status:', data.device);
        }
        else {
          setEsp32Status("Desconectado");
        }

      } catch (error) {
        setEsp32Status("Desconectado");
        console.error('Error fetching ESP32 status:', error);
      }
    };

    fetchEsp32Status();
    
    // Verificamos el estado cada 5 segundos
    const intervalId = setInterval(fetchEsp32Status, 5000);

    return () => clearInterval(intervalId);
  }, []);

  return (
    <AppBar position="static" sx={{ bgcolor: '#0a0a0a', borderBottom: '2px solid #333' }}>
      <Toolbar sx={{ minHeight: '70px', flexWrap: 'wrap', py: { xs: 1, sm: 0 } }}>
        <Typography variant="h6" sx={{ flexGrow: 1, fontWeight: 'bold', letterSpacing: { xs: 0, sm: 1 }, color: '#e0e0e0', fontSize: { xs: '1rem', sm: '1.25rem' } }}>
          Carro Transbordador <Box component="span" sx={{ display: { xs: 'none', sm: 'inline' } }}>de Autoclave</Box>
        </Typography>
        
        <Box sx={{ display: 'flex', gap: 2, mr: { xs: 1, sm: 6 } }}>
          <Button 
            component={Link} to="/" 
            variant={location.pathname === '/' ? 'contained' : 'text'}
            color="inherit"
          >Dashboard</Button>
          {/* <Button 
            component={Link} to="/fallas" 
            variant={location.pathname === '/fallas' ? 'contained' : 'text'}
            color="inherit"
          >Fallas</Button> */}
        </Box>

        <Box sx={{ display: 'flex', alignItems: 'center', gap: { xs: 1, sm: 3 } }}>
          <Typography variant="button" sx={{ color: '#9c9c9c', display: { xs: 'none', md: 'block' } }}>Estado: Detenido </Typography>
          <Chip size="small" label={`ESP32: ${esp32Status}`} sx={{ bgcolor: esp32Status === "Conectado" ? '#4caf50' : '#b90e0e', color: '#fff', borderRadius: 1, fontSize: { xs: '0.7rem', sm: '0.8125rem' } }} />
        </Box>
      </Toolbar>
    </AppBar>
  );
}